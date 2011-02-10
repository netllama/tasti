<?php
	include ("tasti.inc") ;
	$isAuthenticated=login_check();
?>
<head>
	<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
	<title>Tasti - Edit Tags</title>
	<link rel="stylesheet" type="text/css" href="css/main.css" />
	
	<script type="text/javascript">
		function toggle(aId) {
			var collection = document.getElementById(aId).getElementsByTagName("input");
			for (var x=0; x<collection.length; x++) {
				if (collection[x].type.toLowerCase() == "checkbox") {
					if (collection[x].checked) {
						collection[x].checked = false;
					}
					else {collection[x].checked = true}
				}
			}
		}
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
