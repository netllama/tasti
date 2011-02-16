<html lang='en'>
<head>
	<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
	<title>Tasti</title>
	<link rel="stylesheet" type="text/css" href="css/main.css" />
</head>
<body>
<?php
	include ("tasti.inc") ;
?>

<div id="wrapper">
        <div id="header">
		<?php
			header0() ;
		?>
	</div>
	<div id="faux">
                <div id="leftcolumn">
			<?php 
			       do_bmarks() ;
			?>
			<div class="clear"></div>
		</div>
		<div id="rightcolumn">
		<?php
			list_tags() ;
		?>
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
</html>
