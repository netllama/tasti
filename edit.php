<html lang='en'>
<?php
	include ("tasti.inc") ;
	$isAuthenticated=login_check();
?>
<head>
	<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
	<title>Tasti - Edit a bookmark</title>
	<link rel="stylesheet" type="text/css" href="css/main.css" />
	<link rel="stylesheet" type="text/css" href="css/jquery.validity.css" />

	<script type="text/javascript" src="js/jquery.js"></script>
	<script type="text/javascript" src="js/jquery.validity.js"></script>
	
	<script type="text/javascript">
    		$(function() { 
       			$("form").validity(function() {
				$("#name")
    					.minLength(2, "Must be at least 2 characters") ;
				$("#url")
    					.match("url","Requires a valid URL (http://...)")
					.minLength(6, "Must be at least 6 characters");				
        		});
    		});
	</script>
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
			edit_bmarks() ;
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
