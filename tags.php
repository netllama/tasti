<html lang='en'>
<head>
	<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
	<title>Tasti - Tags</title>
	<link rel="stylesheet" type="text/css" href="css/main.css" />
	<script src="js/sorttable.js"></script>
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
			       show_tags() ;
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
