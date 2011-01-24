<?php
	include ("tasti.inc") ;
	$isAuthenticated=login_check();
?>
<head>
	<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
	<title>Tasti - Account Management</title>
	<link rel="stylesheet" type="text/css" href="css/main.css" />
	<link rel="stylesheet" type="text/css" href="css/jquery.validity.css" />

	<script type="text/javascript" src="js/jquery.js"></script>
	<script type="text/javascript" src="js/jquery.validity.js"></script>
	
	<script type="text/javascript">
    		$(function() { 
       			$("form").validity(function() {
				if($("#password0").val().length){
					$("#password0")
    						.minLength(6, "Must be at least 6 characters") ;
				}
				if($("#password1").val().length){
					$("#password1")
    						.minLength(6, "Must be at least 6 characters") ;
				}
				if($("#email").val().length){
					$("#email")
    						.match("email","Requires a valid email address")
						.minLength(5, "Must be at least 5 characters");
				}
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
