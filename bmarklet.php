<?php
	include ("tasti.inc") ;
	$isAuthenticated=login_check();
?>
<head>
	<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
	<title>Tasti - Account Management</title>
	<link rel="stylesheet" type="text/css" href="css/main.css" />
</head>
<body>
<div id="wrapper">
        <div id="header">
		<?php
			header0($isAuthenticated) ;
		?>
	</div>
	<div id="faux">
                <div id="leftcolumn">
		<?php
			account_mgmt() ;
		?>
			<div class="clear"></div>
		</div>
		<div id="rightcolumn">
			<div class="clear"></div>
		</div>  
	</div>	       
	<div id="footer">
		<?php
			footer() ;
		?>
	</div>   
</div>
</body>
