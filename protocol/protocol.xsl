<?xml version="1.0" ?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html" indent="yes" encoding="UTF-8"/>
<xsl:preserve-space elements="copyright"/>
<xsl:template match="/">
<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html;charset=utf-8" />
		<link href="protocol.css" rel="stylesheet" type="text/css" />
		<title>Cloud</title>
	</head>
	<body>
		<h1>Cloud Protocol Specification</h1>
		<xsl:apply-templates select="protocol/copyright"/>
	</body>
</html>
</xsl:template>

<xsl:template match="copyright">
<div>
	<pre class="copyright">
		<xsl:value-of select="." disable-output-escaping="yes"/>
	</pre>
</div>
</xsl:template>
</xsl:stylesheet>
