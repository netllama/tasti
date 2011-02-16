<html lang='en'>
<head>
	<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
	<title>Tasti - Register</title>
	<link rel="stylesheet" type="text/css" href="css/main.css" />
	<link rel="stylesheet" type="text/css" href="css/jquery.validity.css" />

	<script type="text/javascript" src="js/jquery.js"></script>
	<script type="text/javascript" src="js/jquery.validity.js"></script>
	
	<script type="text/javascript">
    		$(function() { 
       			$("form").validity(function() {
        	    		$("#username")
    					.minLength(5, "Must be at least 5 characters") ;
				$("#password0")
    					.minLength(6, "Must be at least 6 characters") ;
				$("#password1")
    					.minLength(6, "Must be at least 6 characters") ;
				$("#email")
    					.match("email","Requires an email address")
					.minLength(5, "Must be at least 5 characters");
        		});
    		});
	</script>
	
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
			       register() ;
			?>
			<div class="clear"></div>
		</div>
		<div id="rightcolumn">
			<?
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
