<?php
/*
 * monit.inc
 *
 * part of pfSense (https://www.pfsense.org)
 * Copyright (c) 2009-2022 Rubicon Communications, LLC (Netgate)
 * All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
require_once("config.inc");
require_once("functions.inc");
require_once("util.inc");
require_once("service-utils.inc");

const MONIT_CONFIG_FILE = "/usr/local/etc/monitrc";

function monit_write_config($monit_config) {
    // $txt = "# This file is generated by the pfSense monit package.\n";
    // $txt .= "# Do not edit this file, it will be overwritten automatically.\n";
    // $txt .= "\n";
    // $txt .= "monit_enable=\"YES\"\n"; // If it had been NO, this function isn't called, so fine to hardcode
    $txt = $monit_config["monitrc"];

	if (!file_put_contents(MONIT_CONFIG_FILE, $txt)) {
		log_error("ERROR: Could not open {" . MONIT_CONFIG_FILE . "} for writing");
		exit;
	}
	if (!chmod(MONIT_CONFIG_FILE, 0700)) {
		log_error("ERROR: Could not set permissions on {" . MONIT_CONFIG_FILE . "}");
		exit;
	}
}
function monit_sync_config() {
	global $config;
	if (is_service_running('monit')) {
		log_error("Stopping service monit");
		stop_service('monit');
	}

	$monit_config = &$config['installedpackages']['monit']['config'][0];
	if ($monit_config['enable'] !== 'on') {
		return;
	}

	monit_write_config($monit_config);

	if (platform_booting()) {
		return;
	}
	log_error("Starting service monit");
	start_service('monit');
}

function monit_install_command() {
	global $config;

	// If installing for the first time, set up the default config
	$monit_config = &$config['installedpackages']['monit']['config'][0];
	if (!is_array($monit_config)) {
		$monit_config = [
			"enable" => "on",
			"monitrc" => "",
		];
	}

	monit_sync_config();
}

function monit_deinstall_command() {
	if (is_service_running('monit')) {
		log_error("Stopping service monit");
		stop_service('monit');
	}
	unlink_if_exists(MONIT_CONFIG_FILE);
}

function monit_validate_form($post, &$input_errors) {
	if (empty($post['monitrc'])) {
		$input_errors[] = gettext('You must enter the monitrc config');
	}
    $out = [];
    if (false === exec("/usr/local/bin/monit -t " . MONIT_CONFIG_FILE, $out)) {
		log_error("ERROR: Could not validate config file " . MONIT_CONFIG_FILE);
		exit;
    }

    if (isset($out[0]) && $out[0] != "Control file syntax OK") {
        foreach ($out as $err_msg) {
            $input_errors[] = gettext($err_msg);
        }
    }
}

?>