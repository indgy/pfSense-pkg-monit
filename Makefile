PORTNAME=	pfSense-pkg-monit
PORTVERSION= 5.30.0
PORTREVISION= 4
CATEGORIES= sysutils
MASTER_SITES=	# empty
DISTFILES= # empty
EXTRACT_ONLY=	# empty

MAINTAINER= coreteam@pfsense.org
COMMENT= pfSense package for monit

LICENSE= APACHE20

USES=

NO_BUILD= yes
NO_MTREE= yes

SUB_FILES= pkg-install pkg-deinstall
SUB_LIST= PORTNAME=${PORTNAME}

do-extract:
	${MKDIR} ${WRKSRC}${DATADIR}

do-install:
	${MKDIR} ${STAGEDIR}${PREFIX}/pkg
	${MKDIR} ${STAGEDIR}${DATADIR}
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/pkg/monit.xml \
		${STAGEDIR}${PREFIX}/pkg
	${INSTALL_DATA} ${FILESDIR}${PREFIX}/pkg/monit.inc \
		${STAGEDIR}${PREFIX}/pkg
	${INSTALL_DATA} ${FILESDIR}${DATADIR}/info.xml ${STAGEDIR}${DATADIR}
	@${REINPLACE_CMD} -i '' -e "s|%%PKGVERSION%%|${PKGVERSION}|" \
		${STAGEDIR}${DATADIR}/info.xml

.include <bsd.port.mk>