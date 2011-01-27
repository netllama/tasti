<?php
// tasti includes

require_once "tasti_config.php" ;

$dbhost=DB_HOST ;
$dbport=DB_PORT ;
$dbname=DB_NAME ;
$dbuser=DB_USER ;
$dbpass=DB_PASS ;

// connect to the database
$db = pg_connect("host=$dbhost port=$dbport dbname=$dbname user=$dbuser password=$dbpass ");
if (!$db) {
	echo '<BR><span class="bad">Cannot connect to the database server.<BR><BR>';
	echo pg_last_error() . '</span><BR><BR>';
}

// check whether the user has logged in 
function auth_check(){
	if(isset($_COOKIE['tasti_username']) && hash_check()){
		$username=strtolower($_COOKIE['tasti_username']) ;
		return array('1',$username);
	}else{
		return array('0','');
	}
}

// page header
function header0($isAuthenticated=2){
	list($auth_check0,$username)=auth_check() ;
	if((($auth_check0 && strlen($username)) || $isAuthenticated=='1') && $isAuthenticated!='3'){
		$header_string='&nbsp;Hi <A HREF="account.php">' . $username . '</a>&nbsp;|&nbsp;<A HREF="login.php?do=1">Logout</a><BR><BR><UL><LI><A HREF="add.php">ADD</a>&nbsp;a&nbsp;bookmark<LI><A HREF="bmarks.php?whose=mine">MY BOOKMARKS</a></UL>' ;
	}else{
		$header_string='&nbsp;<A HREF="login.php?do=0">Login</a>&nbsp;|&nbsp;<A HREF="register.php">Register</a>&nbsp;' ;
	}
	echo '<span class="header_class1"><TABLE width="100%"><TR><TD><A HREF="index.php"><IMG SRC="tasti-logo.png"></a></TD><TD>&nbsp;</TD><TD align="right" valign="top">' . $header_string  . '</TD></TR></TABLE></span>' ;
}

// page footer
function footer(){
	$year=date('Y') ;
	echo '<span class="footer_class1"><CENTER><A HREF="index.php">HOME</a>&nbsp;|&nbsp;<A HREF="mailto:' . ADMIN_EMAIL . '">HELP</a><BR>Tasti is licensed under the <A HREF="http://www.gnu.org/licenses/gpl.html">GPL</a>.  Copyright ' . $year . '<BR></CENTER></span>' ;
}

// manage bookmarks
function do_bmarks(){
	global $db ;
	if(isset($_GET['func']) && isset($_COOKIE['tasti_username']) && hash_check() && isset($_GET['id'])){
		$username=$_COOKIE['tasti_username'] ;
		$id=$_GET['id'] ;
		if($_GET['func']=='del'){
			$old_bmark_sql="SELECT created,url,notes,name FROM bmarks WHERE id='$id' LIMIT 1" ;
			$old_bmark_qry=pg_query($db,$old_bmark_sql) ;
			if(!$old_bmark_qry){
				echo '<span class="bad">Old Bookmark query FAILED: <BR>' ;
				echo pg_last_error() . '</span><BR><BR>';
				exit() ;
			}
			$old_created=pg_result($old_bmark_qry,0,0) ;
			$old_url=pg_result($old_bmark_qry,0,1) ;
			$old_notes=pg_result($old_bmark_qry,0,2) ;
			$old_name=pg_result($old_bmark_qry,0,3) ;
			
			$bmark_del_sql="DELETE FROM bmarks WHERE owner='$username' AND url='$old_url' AND name='$old_name' AND notes='$old_notes'" ;
			$bmark_del_qry=pg_query($db,$bmark_del_sql) ;
			if(!$bmark_del_qry){
				echo '<span class="bad">Bookmark deletion FAILED: <BR>' ;
                		echo pg_last_error() . '</span><BR><BR>';
			}else{
				echo '<span class="huge">Bookmark successfully deleted</span><BR><BR>' ;
			}
		}
	}else{
		show_bmarks() ;
	}
}

// edit bmarks form
function edit_bmarks_form(){
	global $db ;
	$username=$_COOKIE['tasti_username'] ;
	$id=$_GET['id'] ;
	if($_GET['func']=='edit'){
		$bmarks_sql="SELECT name,url,notes FROM bmarks WHERE id='$id' AND owner='$username' LIMIT 1" ;
		$bmarks_qry=pg_query($db,$bmarks_sql) ;
		if(!$bmarks_qry){
			echo '<span class="bad">Bookmark query FAILED: <BR>' ;
                	echo pg_last_error() . '</span><BR><BR>';
                	exit() ;
		}else{
			$name=pg_result($bmarks_qry,0,0) ;
			$url=pg_result($bmarks_qry,0,1);
			$notes=pg_result($bmarks_qry,0,2);

			$tags_sql="SELECT tag FROM bmarks WHERE owner='$username' AND url='$url' ORDER BY tag" ;
			$tags_qry=pg_query($db,$tags_sql) ;
			if(!$tags_qry){
				echo '<span class="bad">Tag query FAILED: <BR>' ;
                		echo pg_last_error() . '</span><BR><BR>';
                		exit() ;
			}else{
				$num_tags=pg_num_rows($tags_qry) ;
				echo '<span class="huge">Edit this bookmark:</span><BR><BR>' ;
				echo '<FORM method="POST" action="edit.php" id="edit_bmark"><TABLE>
					<TR><TD>Name/Description*:&nbsp;</TD><TD>&nbsp;</TD><TD><INPUT TYPE="text" NAME="name" id="name" size="55" VALUE="' . $name . '"></TD></TR>
					<TR><TD>URL*:&nbsp;</TD><TD>&nbsp;</TD><TD><INPUT TYPE="text" NAME="url" id="url" size="55" VALUE="' . $url . '"></TD></TR>
					<TR><TD>Notes:&nbsp;</TD><TD>&nbsp;</TD><TD><INPUT TYPE="text" NAME="notes" id="notes" size="55" VALUE="' . $notes . '"></TD></TR>
					<TR><TD>Tags:&nbsp;</TD><TD>&nbsp;</TD><TD><INPUT TYPE="text" NAME="tags" id="tags" size="55" VALUE="' ;
				for($lt=0; $lt < $num_tags ; $lt++){
					$tag=trim(pg_result($tags_qry,$lt,0)) ;
					echo $tag . ' ' ;
				}
				echo '"></TD></TR><TR><TD>&nbsp;</TD><TD>&nbsp;</TD><TD>&nbsp;</TD><TD>&nbsp;</TD></TR>
					<TR><TD>&nbsp;</TD><TD><DIV class="submit"><INPUT type="submit" value="Submit" /></TD><TD>&nbsp;</TD></TR>
				</TABLE>
				<INPUT type="hidden" id="id" name="id" value="' . $id . '">
				</FORM><BR><span class="tiny">&nbsp;* Required field</span><BR><BR>' ;

				// display a clickable list of the user's tags that will be added to the Tags form field
				$all_tags_sql="SELECT tag FROM tags WHERE owner='$username' ORDER BY tag" ;
				$all_tags_qry=pg_query($db,$all_tags_sql) ;
				if(!$all_tags_qry){
					echo '<span class="bad">Tag query FAILED: <BR>' ;
                                        echo pg_last_error() . '</span><BR><BR>';
				}
				$num_users_tags=pg_num_rows($all_tags_qry) ;
        			if($num_users_tags>0){
                			echo '<span class="big">Click below to add your pre-existing tags to the bookmark above:</span><BR><BR>' ;
                			$row_tag_count=0 ;
                			for ($tagid=0; $tagid < $num_users_tags ; $tagid++){
                        			$tagname=htmlentities(trim(pg_result($all_tags_qry,$tagid,0))) ;
                        			echo '<A onclick="document.getElementById(\'tags\').value=document.getElementById(\'tags\').value + \' \' + \'' . $tagname . '\';">' . $tagname . '</a>&nbsp;&nbsp;' ;
                        			$row_tag_count++ ;
                        			// only display 10 tags per row
                        			if($row_tag_count>10){
                                			echo '<BR>' ;
                                			$row_tag_count=0 ;
                        			}
                			}
                			echo '<BR><BR><BR>' ;
        			}	
			}
		}
	}else{
		echo '<span class="bad">Invalid function, or not your bookmark.</span><BR><BR><BR>' ;
	}
}

// process POST'd bookmark edit data
function edit_bmarks_process(){
	global $db ;
	$id=$_POST['id'] ;
	$username=$_COOKIE['tasti_username'] ;
	$bmark_name=trim($_POST['name']) ;
	$bmark_url=trim($_POST['url']) ;
	if(isset($_POST['notes']) && strlen(trim($_POST['notes']))){
		$bmark_notes=trim($_POST['notes']);
	}else{
		$bmark_notes='' ;
	}

	if(isset($_POST['tags']) && strlen(trim($_POST['tags']))){
		$bmark_tags_string=trim($_POST['tags']);
		$bmark_tags_array_dupes=explode(" ",$bmark_tags_string) ;
		$bmark_tags_array=array_unique($bmark_tags_array_dupes) ;
	}

	$old_bmark_sql="SELECT created,url,notes,name FROM bmarks WHERE id='$id' LIMIT 1" ;
	$old_bmark_qry=pg_query($db,$old_bmark_sql) ;
	if(!$old_bmark_qry){
		echo '<span class="bad">Old Bookmark query FAILED: <BR>' ;
		echo pg_last_error() . '</span><BR><BR>';
		exit() ;
	}
	$old_created=pg_result($old_bmark_qry,0,0) ;
	$old_url=pg_result($old_bmark_qry,0,1) ;
	$old_notes=pg_result($old_bmark_qry,0,2) ;
	$old_name=pg_result($old_bmark_qry,0,3) ;

	$tr_status=0 ;
	$tr_begin=pg_query($db, 'BEGIN;');
	$bmark_del_sql="DELETE FROM bmarks WHERE owner='$username' AND url='$old_url' AND name='$old_name' AND notes='$old_notes'" ;
	$bmark_del_qry=pg_query($db,$bmark_del_sql) ;
	if(!$bmark_del_qry){
		echo '<span class="bad">Bookmark delete FAILED: <BR>' ;
		echo pg_last_error() . '</span><BR><BR>';
		$tr_end = pg_query($db,'ROLLBACK;');
		exit() ;
	}
	// insert tags first, if they do not already exist
	foreach($bmark_tags_array as $tag){
		$tag=trim($tag) ;
		$userHasTag_sql="SELECT count(id) FROM tags WHERE owner='$username' AND tag='$tag'" ;
		$userHasTag_query=pg_query($db,$userHasTag_sql) ;
		if(!$userHasTag_query){
			echo '<span class="bad">Tag query FAILED: <BR>' ;
			echo pg_last_error() . '</span><BR><BR>';
			$tr_end = pg_query($db,'ROLLBACK;');
			exit() ;
		}else{
			$tag_count=pg_result($userHasTag_query,0,0) ;
			if($tag_count==0){
				// if the tag doesn't exist, insert it as long as its not just white space
				if(strlen(trim($tag))){
					$addUserTag_sql="INSERT INTO tags (owner,tag) VALUES ('$username','$tag')" ;
					$addUserTag_qry=pg_query($db,$addUserTag_sql) ;
					if(!$addUserTag_qry){
						echo '<span class="bad">Tag INSERT FAILED: <BR>' ;
						echo pg_last_error() . '</span><BR><BR>';
						$tr_end = pg_query($db,'ROLLBACK;');
						exit() ;
					}
				}
			}
		}
		$addBmark_sql="INSERT INTO bmarks (owner,url,notes,tag,name,created) VALUES ('$username','$bmark_url','$bmark_notes','$tag','$bmark_name','$old_created')" ;
		$addBmark_qry=pg_query($db,$addBmark_sql) ;
		if(!$addBmark_qry){
			echo '<span class="bad">Bookmark INSERT FAILED: <BR>' ;
			echo pg_last_error() . '</span><BR><BR>';
			$tr_end = pg_query($db,'ROLLBACK;');
			exit() ;
		}
	}

	// rollback everything if any part of the transaction failed
	if($tr_status){
		$tr_end = pg_query($db,'ROLLBACK;');
		exit();
	}
	$tr_end=pg_query($db,'COMMIT;');
	echo 'Bookmark <B>( ' . htmlentities($bmark_name) . ' )</B> successfully updated!<BR><BR>' ;
}

// edit bookmarks
function edit_bmarks(){
	if(isset($_GET['func']) && isset($_COOKIE['tasti_username']) && hash_check()){
		// display the editing form
		edit_bmarks_form() ;
	}elseif(isset($_POST['name']) && isset($_COOKIE['tasti_username'])){
		// process the data POST'd from the edit form
		edit_bmarks_process() ;
	}else{
		echo '<span class="bad">Invalid function, or not your bookmark.</span><BR><BR><BR>' ;
	}
}

// show bookmarks
function show_bmarks(){
	global $db ;
	$a5_marker='' ; $a10_marker='' ; $a15_marker='' ; $a20_marker='' ; $a30_marker='' ;
	if(isset($_GET['num']) && is_numeric($_GET['num'])){
		$user_num_bmarks=$_GET['num'] ;
		$limit_sql=' LIMIT ' . $user_num_bmarks ;
		if($user_num_bmarks=='5'){
			$a5_marker='*&nbsp;' ;
		}elseif($user_num_bmarks=='10'){
			$a10_marker='*&nbsp;' ;
		}elseif($user_num_bmarks=='15'){
			$a15_marker='*&nbsp;' ;
		}elseif($user_num_bmarks=='20'){
			$a20_marker='*&nbsp;' ;
		}elseif($user_num_bmarks=='30'){
			$a30_marker='*&nbsp;' ;
		}
	}else{
		$user_num_bmarks='' ;
		$limit_sql=' LIMIT 20' ;
	}
	if(isset($_GET['whose']) && $_GET['whose']=='mine' && hash_check()){
		$username=$_COOKIE['tasti_username'] ;
		$url_get_base=basename($_SERVER['REQUEST_URI']) . '&' ;
		$num_bmarks_menu_offset='77' ;
		$bmarks_intro='<span class="huge">Your bookmarks:</span><BR><BR>' ;
		$bmarks_sql="SELECT distinct ON (url) url,id,date(last_update) AS last_update,notes,name,owner FROM bmarks WHERE owner='$username' ORDER BY url,last_update" . $limit_sql;
	}else{
		$num_bmarks_menu_offset='73' ;
		$url_get_base=basename($_SERVER['REQUEST_URI']) . '?' ;
		$bmarks_intro='<span class="huge">Recent bookmarks:</span><BR><BR>' ;
		$bmarks_sql="SELECT distinct ON (url) url,id,date(last_update) AS last_update,notes,name,owner FROM bmarks ORDER BY url,last_update" . $limit_sql;
	}
	$bmarks_qry=pg_query($db,$bmarks_sql) ;
	if(!$bmarks_qry){
		echo '<span class="bad">Bookmark query FAILED: <BR>' ;
                echo pg_last_error() . '</span><BR><BR>';
                exit() ;
	}else{
		$num_bmarks=pg_num_rows($bmarks_qry) ;
		if($num_bmarks>0){
			echo '<TABLE><TR><TD>' . $bmarks_intro . '</TD>' ;
			// only render the bmarks/page menu on the 'MY BOOKMARKS' page
			if($num_bmarks_menu_offset=='77'){
				echo '<TD width="' . $num_bmarks_menu_offset . '%">&nbsp;</TD><TD align="center" valign="top"><div id="menu">
				<UL id="item1">
				<LI class="top"><B>Bookmarks/page</B></LI>
					<LI class="item"><A HREF="' . $url_get_base . 'num=5">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;' . $a5_marker . '5&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</a></LI>
					<LI class="item"><A HREF="' . $url_get_base . 'num=10">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;' . $a10_marker . '10&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</a></LI>
					<LI class="item"><A HREF="' . $url_get_base . 'num=15">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;' . $a15_marker . '15&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</a></LI>
					<LI class="item"><A HREF="' . $url_get_base . 'num=20">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;' . $a20_marker . '20&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</a></LI>
					<LI class="item"><A HREF="' . $url_get_base . 'num=30">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;' . $a30_marker . '30&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</a></LI></UL>
				</DIV>&nbsp;</TD>' ;
			}
			echo '</TR></TABLE>' ;
			for($lt=0; $lt < $num_bmarks ; $lt++){
				$url=pg_result($bmarks_qry,$lt,0) ;
				$id=pg_result($bmarks_qry,$lt,1) ;
				$last_update=pg_result($bmarks_qry,$lt,2) ;
				$notes=htmlentities(pg_result($bmarks_qry,$lt,3)) ;
				$name=htmlentities(pg_result($bmarks_qry,$lt,4)) ;
				$owner=pg_result($bmarks_qry,$lt,5) ;
				
				$tags_sql="SELECT tag FROM bmarks WHERE url='$url' " ;
				if(isset($username)){
					$tags_sql.="AND owner='$username' " ;
				}
				$tags_sql.="AND length(tag)>0 GROUP BY tag ORDER BY tag LIMIT 15" ;
				$tags_qry=pg_query($db,$tags_sql) ;
				if(!$tags_qry){
					echo '<span class="bad">Bookmark query FAILED: <BR>' ;
                			echo pg_last_error() . '</span><BR><BR>';
                			exit() ;
				}else{
					if(strlen($notes)){
						$notes_string='<BR><span class="small"><B>' . $notes . '</B></span>' ;
					}else{
						$notes_string='' ;
					}
					if(isset($username)){
						$bmark_user_edit_string='<BR><A HREF="edit.php?id=' . $id . '&func=edit"><span class="normal"><B>EDIT</B></a>&nbsp;|&nbsp;<A HREF="bmarks.php?id=' . $id . '&func=del"><B>DELETE</B></a>&nbsp;</span>' ;
					}else{
						$bmark_user_edit_string='' ;
					}
				
					echo '<TABLE><TR><TD valign="top"><span class="big">' . $last_update . '&nbsp;&nbsp;&nbsp;</span></TD><TD width="65%"><span class="big"><A HREF="' . $url . '">' . $name . '</a></span>' . $notes_string . $bmark_user_edit_string . '</TD>'  ;
					
					$num_tags=pg_num_rows($tags_qry) ;
					if($num_tags>0){
						echo '<TD width="95%" valign="top" align="right">' ;
						$tag_counter=0 ;
						for($counter=0; $counter < $num_tags ; $counter++){
							$tag=pg_result($tags_qry,$counter,0) ;
							$tag_sql="SELECT id FROM tags WHERE tag='$tag' " ;
							if(isset($username)){
								$tag_sql.="AND owner='$username' " ;
							}
							$tag_sql.=" ORDER BY id LIMIT 1" ;
							$tag_qry=pg_query($db,$tag_sql) ;
							$tag_id=pg_result($tag_qry,0,0) ;
							echo '<A HREF="tags.php?id=' . $tag_id . '">' . $tag . '</a>&nbsp;&nbsp;' ;
							$tag_counter++ ;
							if($tag_counter>4){
								$tag_counter=0 ;
								echo '<BR>' ;
							}
						}
						echo '&nbsp;</TD>' ;
					}
					echo '</TR></TABLE><HR><BR>' ;
					
					// pagination
				}
			}
		}else{
			echo '<BR>There are currently no bookmarks<BR><BR>' ;
		}
	}
}

// show tags
function show_tags(){
	global $db ;
	if(isset($_GET['id'])){
		$tagid=$_GET['id'] ;
	}

	$tags_sql="SELECT tag FROM tags " ;

	if(isset($_COOKIE['tasti_username']) && hash_check()){
		$username=$_COOKIE['tasti_username'] ;
		if(isset($_GET['mine']) && $_GET['mine']=='yes'){
			$show_mine="AND owner='$username'" ;
			$url_get_base=basename($_SERVER['REQUEST_URI']) . '&' ;
			$num_bmarks_menu_offset='47' ;
		}else{
			$show_mine='' ;
		}
		if(isset($tagid)){
			$tagid=$_GET['id'] ;
			//$tags_sql.="WHERE owner='$username' AND id='$tagid' LIMIT 1" ;
			$tags_sql.="WHERE id='$tagid' " . $show_mine . " LIMIT 1" ;
		}else{
			//$tags_sql.="WHERE owner='$username' ORDER BY id DESC LIMIT 1" ;
			$tags_sql.="WHERE true=true " . $show_mine . " ORDER BY id DESC LIMIT 1" ;
		}
	}else{
		if(isset($tagid)){
                        $tagid=$_GET['id'] ;
                        $tags_sql.="WHERE id='$tagid' LIMIT 1" ; 
                }else{
                        $tags_sql.="ORDER BY id DESC LIMIT 1" ;
                }
	}
	$tags_qry=pg_query($db,$tags_sql) ;
	if(!$tags_qry){
		echo '<span class="bad">Tag query FAILED: <BR>' ;
                echo pg_last_error() . '</span><BR><BR>';
                exit() ;	
	}else{
		$tag_count=pg_num_rows($tags_qry) ;
		if($tag_count>0){
			$a5_marker='' ; $a10_marker='' ; $a15_marker='' ; $a20_marker='' ; $a30_marker='' ;
			if(isset($_GET['num']) && is_numeric($_GET['num'])){
				$user_num_bmarks=$_GET['num'] ;
				$limit_sql=' LIMIT ' . $user_num_bmarks ;
				if($user_num_bmarks=='5'){
					$a5_marker='*&nbsp;' ;
				}elseif($user_num_bmarks=='10'){
					$a10_marker='*&nbsp;' ;
				}elseif($user_num_bmarks=='15'){
					$a15_marker='*&nbsp;' ;
				}elseif($user_num_bmarks=='20'){
					$a20_marker='*&nbsp;' ;
				}elseif($user_num_bmarks=='30'){
					$a30_marker='*&nbsp;' ;
				}
			}else{
				$user_num_bmarks='' ;
				$limit_sql=' LIMIT 20' ;
			}
			$tag=htmlentities(pg_result($tags_qry,0,0)) ;
			$bmark_sql="SELECT id,date(last_update) as last_update,owner,url,notes,name FROM bmarks WHERE tag='$tag' " ;
			if(isset($username) && isset($_GET['mine']) && $_GET['mine']=='yes'){
				$bmark_sql.=" AND owner='$username' " ;
			}
			$bmark_sql.='ORDER BY owner,last_update,name' . $limit_sql ;
			$bmark_qry=pg_query($db,$bmark_sql) ;
			if(!$bmark_qry){
				echo '<span class="bad">Bookmark query FAILED: <BR>' ;
                		echo pg_last_error() . '</span><BR><BR>';
			}else{
				$num_bmarks=pg_num_rows($bmark_qry) ;
				if($num_bmarks>0){
					echo '<TABLE width="100%"><TR><TD width="85%"><span class="huge">Bookmarks tagged with <B>' . $tag . '</B></span></TD>' ;
					// only render the bmarks/page menu on the 'MY BOOKMARKS' page
					if($num_bmarks_menu_offset=='47'){
						echo '<TD width="70%" align="center" valign="top"><div id="menu">
						<UL id="item1">
						<LI class="top"><B>Bookmarks/page</B></LI>
							<LI class="item"><A HREF="' . $url_get_base . 'num=5">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;' . $a5_marker . '5&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</a></LI>
							<LI class="item"><A HREF="' . $url_get_base . 'num=10">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;' . $a10_marker . '10&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</a></LI>
							<LI class="item"><A HREF="' . $url_get_base . 'num=15">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;' . $a15_marker . '15&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</a></LI>
							<LI class="item"><A HREF="' . $url_get_base . 'num=20">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;' . $a20_marker . '20&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</a></LI>
							<LI class="item"><A HREF="' . $url_get_base . 'num=30">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;' . $a30_marker . '30&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</a></LI></UL>
						</DIV></TD>' ;
					}
					echo '</TR></TABLE>' ;
					echo '<HR><BR>' ;
					for($lt=0; $lt < $num_bmarks ; $lt++){
						$id=pg_result($bmark_qry,$lt,0) ;
						$last_update=pg_result($bmark_qry,$lt,1) ;
						$owner=pg_result($bmark_qry,$lt,2) ;
						$url=pg_result($bmark_qry,$lt,3) ;
						$notes=htmlentities(pg_result($bmark_qry,$lt,4)) ;
						$name=htmlentities(pg_result($bmark_qry,$lt,5)) ;
					
						if(strlen($notes)){
							$notes_string='<BR><span class="small"><B>' . $notes . '</B></span>' ;
						}else{
							$notes_string='' ;
						}
						if(isset($username) && $username==$owner){
							$bmark_user_edit_string='<BR><A HREF="edit.php?id=' . $id . '&func=edit"><span class="normal"><B>EDIT</B></a>&nbsp;|&nbsp;<A HREF="bmarks.php?id=' . $id . '&func=del"><B>DELETE</B></a>&nbsp;</span>' ;
						}else{
							$bmark_user_edit_string='' ;
						}
						echo '<TABLE><TR><TD valign="top"><span class="big">' . $last_update . '&nbsp;&nbsp;&nbsp;</span></TD><TD><span class="big"><A HREF="' . $url . '">' . $name . '</a></span>' . $notes_string . $bmark_user_edit_string . '</TD>'  ;
						echo '</TR></TABLE><HR><BR>' ;
					}
				}else{
					echo '<BR>There are currently no bookmarks associated with the ' . $tag . ' tag.<BR><BR><BR>' ;
				}
			}
		}else{
			echo 'No tags selected.<BR><BR><BR>' ;
		}
	}
}

// add the bookmark to the database
function add_bmark_db($username){
	global $db ;
	$bmark_name=$_POST['name'] ;
	$bmark_url=$_POST['url'] ;
	if(isset($_POST['notes']) && strlen(trim($_POST['notes']))){
		$bmark_notes=trim($_POST['notes']);
	}else{
		$bmark_notes='' ;
	}
	if(isset($_POST['tags']) && strlen(trim($_POST['tags']))){
		$bmark_tags_string=trim($_POST['tags']);
		$bmark_tags_array_dupes=explode(" ",$bmark_tags_string) ;
		$bmark_tags_array=array_unique($bmark_tags_array_dupes) ;
	}
	
	$tr_status=0 ;
	$tr_begin=pg_query($db, 'BEGIN;');
		// insert tags first, if they do not already exist
		foreach($bmark_tags_array as $tag){
			$tag=trim($tag) ;
			$userHasTag_sql="SELECT count(id) FROM tags WHERE owner='$username' AND tag='$tag'" ;
			$userHasTag_query=pg_query($db,$userHasTag_sql) ;
			if(!$userHasTag_query){
				echo '<span class="bad">Tag query FAILED: <BR>' ;
				echo pg_last_error() . '</span><BR><BR>';
				$tr_end = pg_query($db,'ROLLBACK;');
				exit() ;
			}else{
				$tag_count=pg_result($userHasTag_query,0,0) ;
				if($tag_count==0){
					// if the tag doesn't exist, insert it
					if(strlen($tag)){
						$addUserTag_sql="INSERT INTO tags (owner,tag) VALUES ('$username','$tag')" ;
						$addUserTag_qry=pg_query($db,$addUserTag_sql) ;
						if(!$addUserTag_qry){
							echo '<span class="bad">Tag INSERT FAILED: <BR>' ;
							echo pg_last_error() . '</span><BR><BR>';
							$tr_end = pg_query($db,'ROLLBACK;');
							exit() ;
						}
					}
				}
			}
			$addBmark_sql="INSERT INTO bmarks (owner,url,notes,tag,name) VALUES ('$username','$bmark_url','$bmark_notes','$tag','$bmark_name')" ;
			$addBmark_qry=pg_query($db,$addBmark_sql) ;
			if(!$addBmark_qry){
				echo '<span class="bad">Bookmark INSERT FAILED: <BR>' ;
				echo pg_last_error() . '</span><BR><BR>';
				$tr_end = pg_query($db,'ROLLBACK;');
				exit() ;
			}
		}
		
		// rollback everything if any part of the transaction failed
		if($tr_status){
			$tr_end = pg_query($db,'ROLLBACK;');
			exit();
		}
	$tr_end=pg_query($db,'COMMIT;');
	echo 'Bookmark <B>( ' . htmlentities($bmark_name) . ' )</B> successfully added!<BR><BR>' ;
}

// add a new bookmark
function add_bmark(){
	if(isset($_COOKIE['tasti_username']) && hash_check()){
		if(isset($_POST['url'])){
			add_bmark_db($_COOKIE['tasti_username']) ;
		}
		add_bmark_form() ;
	}else{
		echo 'Only users who have <A HREF="login.php?do=0">logged in</a> may add new bookmarks.<BR>' ;
	}
}

// add a new bookmark form
function add_bmark_form($edit=null){
	global $db ;
	$username=$_COOKIE['tasti_username'] ;
	$tags_sql="SELECT tag FROM tags WHERE owner='$username' ORDER BY tag LIMIT 100" ;
	$tags_query=pg_query($db,$tags_sql) ;
	$num_users_tags=pg_num_rows($tags_query) ;
	
	echo '<span class="huge">Add a new bookmark to <B>Tasti</B>:</span><BR><BR>' ;
	echo '<FORM method="POST" action="add.php" id="add_bmark"><TABLE>
		<TR><TD>Name/Description*:&nbsp;</TD><TD>&nbsp;</TD><TD><INPUT TYPE="text" NAME="name" id="name" size="55"></TD></TR>
		<TR><TD>URL*:&nbsp;</TD><TD>&nbsp;</TD><TD><INPUT TYPE="text" NAME="url" id="url" size="55"></TD></TR>
		<TR><TD>Notes:&nbsp;</TD><TD>&nbsp;</TD><TD><INPUT TYPE="text" NAME="notes" id="notes" size="55"></TD></TR>
		<TR><TD>Tags:&nbsp;</TD><TD>&nbsp;</TD><TD><INPUT TYPE="text" NAME="tags" id="tags" size="55"></TD></TR>
		<TR><TD>&nbsp;</TD><TD>&nbsp;</TD><TD>&nbsp;</TD><TD>&nbsp;</TD></TR>
		<TR><TD>&nbsp;</TD><TD><DIV class="submit"><INPUT type="submit" value="Add" /></TD><TD>&nbsp;</TD></TR>
	</TABLE></FORM><BR><span class="tiny">&nbsp;* Required field</span><BR><BR>' ;
	
	// display a clickable list of the user's tags that will be added to the Tags form field
	if($num_users_tags>0){
		echo '<span class="big">Click below to add your pre-existing tags to the new bookmark above:</span><BR><BR>' ;
		$row_tag_count=0 ;
		for ($tagid=0; $tagid < $num_users_tags ; $tagid++){
			$tagname=htmlentities(trim(pg_result($tags_query,$tagid,0))) ;
			echo '<A onclick="document.getElementById(\'tags\').value=document.getElementById(\'tags\').value + \' \' + \'' . $tagname . '\';">' . $tagname . '</a>&nbsp;&nbsp;' ;
			$row_tag_count++ ;
			// only display 1 tags per row
			if($row_tag_count>10){
				echo '<BR>' ;
				$row_tag_count=0 ;
			}
		}
		echo '<BR><BR><BR>' ;
	}	
}

// list tags - only the user's if logged in, most recently added otherwise
function list_tags (){
	global $db ;
	if(isset($_COOKIE['tasti_username']) && hash_check()){
		$username=$_COOKIE['tasti_username'] ;
		$tags_sql="SELECT id,tag FROM tags WHERE owner='$username' ORDER BY tag" ;
	}else{
		$tags_sql="SELECT id,tag FROM tags ORDER BY last_update,tag LIMIT 50" ;
	}
	$tags_qry=pg_query($db,$tags_sql) ;
	if(!$tags_qry){
                echo '<span class="bad">Tags query FAILED: <BR>' ;
                echo pg_last_error() . '</span><BR><BR>';
                return 1 ;
        }else{
		if(isset($username)){
			$tag_string='<span class="big"><B>Your Tags</B></span><BR><BR>' ; 
			$show_mine='&mine=yes' ;
		}else{
			$tag_string='<span class="big"><B>Recent Tags</B></span><BR><BR>' ;
			$show_mine='' ;
		}
		$num_tags=pg_num_rows($tags_qry) ;
		if($num_tags>0){
			echo $tag_string ;
			for ($tagid=0 ; $tagid < $num_tags ; $tagid++){
				$id=pg_result($tags_qry,$tagid,0) ;
				$tag=htmlentities(pg_result($tags_qry,$tagid,1)) ;
				echo '&nbsp;&nbsp;<A HREF="tags.php?id=' . $id . $show_mine . '">' . $tag . '</a>&nbsp;<BR>' ;
			}
		}
		echo '<BR>' ;	
	}
}

// account details form
function account_details_form (){
	global $db ;
	$name='' ; $email='' ;
	$username=$_COOKIE['tasti_username'] ;
	$account_sql="SELECT name,email FROM users WHERE username='$username' LIMIT 1" ;
	$account_query=pg_query($db,$account_sql) ;
	$name=pg_result($account_query,0,0) ;
	$email=pg_result($account_query,0,1) ;
	echo '<div id="content"><span class="big">Please edit the fields that you wish to change for your <B>Tasti</B> account:</span><BR><BR>' ;
		echo '<FORM method="POST" action="account.php" id="register"><TABLE>
			<TR><TD><label for="password0">Change your Password (at least 6 characters):&nbsp;</label></TD><TD>&nbsp;</TD><TD><INPUT NAME="password0" TYPE="password" id="password0" /></TD></TR>
			<TR><TD><label for="password1">Enter the new password again:&nbsp;</label></TD><TD>&nbsp;</TD><TD><INPUT NAME="password1" TYPE="password" id="password1" /></TD></TR>
			<TR><TD><label for="fullname">Update your full name:&nbsp;</label></TD><TD>&nbsp;</TD><TD><INPUT NAME="fullname" TYPE="text" id="fullname" value="' . $name . '" /></TD></TR>
			<TR><TD><label for="email">Update your email address:&nbsp;</label></TD><TD>&nbsp;</TD><TD><INPUT NAME="email" TYPE="text" id="email" value="' . $email . '" /></TD></TR>
			<TR><TD>&nbsp;</TD><TD>&nbsp;</TD><TD>&nbsp;</TD></TR>
			<TR><TD>&nbsp;</TD><TD><DIV class="submit"><INPUT type="submit" value="Submit" /></TD><TD>&nbsp;</TD></TR>
		</TABLE></FORM><BR></div>' ;
}

// generate the tabs for the acct mgmt page
function generate_tabs (){
	$pagename=substr($_SERVER["SCRIPT_NAME"],strrpos($_SERVER["SCRIPT_NAME"],"/")+1) ;
	if($pagename=='account.php'){
		$accountSelected='id="selected"' ;
	}else{
		$accountSelected='' ;
	}
	if($pagename=='import.php'){
		$importSelected='id="selected"' ;
	}else{
		$importSelected='' ;
	}
	echo '<div id="tabs"><ul>
		<li ' . $accountSelected . '><a href="account.php">Details</a></li><!-- these comments between lis solve a bug in IE that prevents spaces appearing between list items that appear on different lines in the source
			--><li ' . $importSelected . '><a href="import.php">Import&nbsp;Bookmarks</a></li><!-- ' ;
			// --><li><a href="#">The Other</a></li><!--
			// --><li><a href="#">Banana</a></li> 
		echo '--></ul>
		</div>' ;
	return $pagename ;
}

// account mgmt
function account_mgmt(){
	if(isset($_COOKIE['tasti_username']) && hash_check()){
		$password='' ; $name='' ; $email='' ;
		$username=$_COOKIE['tasti_username'] ;
		echo '<span class="huge"><B>Tasti Account Management</B></span><BR><BR>' ;
				
		global $db ;
		$account_sql='UPDATE users SET ' ;
		if(isset($_POST['password0']) && isset($_POST['password1']) && trim($_POST['password0'])==trim($_POST['password1']) && strlen(trim($_POST['password0']))){
			$password=sha1(trim($_POST['password0']));
			$account_sql.="password='$password', " ;
		}
		if(isset($_POST['fullname']) && strlen(trim($_POST['fullname']))){
			$name=trim($_POST['fullname']);
			$account_sql.="name='$name', " ;
		}
		if(isset($_POST['email'])){
			$email=trim($_POST['email']) ;
			$account_sql.="email='$email', " ;
		}
		if(strlen($password) || strlen($name) || strlen($email)){
			$account_sql.="username='$username' WHERE username='$username'" ;
			$account_query=pg_query($db,$account_sql) ;
			if(!$account_query){
				echo '<span class="bad">Account update FAILED: <BR>' ;
				echo pg_last_error() . '</span><BR><BR>';
			}else{
				echo '<span class="big"><B><i>Update Successful</i></B></span><BR><BR>' ;
			}
		}
		$pagename=generate_tabs() ;
		if($pagename=='account.php'){
			account_details_form() ;
		}elseif($pagename=='import.php'){
			echo '<BR><B>Coming Soon</B><BR><BR>' ;
		}else{
			echo '<BR>Unknown function<BR><BR>' ;
		}
	}else{
		echo '<BR>This page is only accessible to users who have <A HREF="login.php?do=0">logged in</a>.<BR><BR>' ;
	}	
}

// registration form
function register_form(){
	echo '<span class="big">Please complete the form to register a new <B>Tasti</B> account:</span><BR><BR>' ;
	echo '<FORM method="POST" action="register.php" id="register"><TABLE><TR>
		<TD><label for="username">Username (letters and/or numbers, at least 5 characters)*:&nbsp;</label></TD><TD>&nbsp;</TD><TD><INPUT TYPE="text" NAME="username" id="username" /></TD></TR>
		<TR><TD><label for="password0">Password (at least 6 characters)*:&nbsp;</label></TD><TD>&nbsp;</TD><TD><INPUT NAME="password0" TYPE="text" id="password0" /></TD></TR>
		<TR><TD><label for="password1">Enter the password again*:&nbsp;</label></TD><TD>&nbsp;</TD><TD><INPUT NAME="password1" TYPE="text" id="password1" /></TD></TR>
		<TR><TD><label for="fullname">Enter your full name:&nbsp;</label></TD><TD>&nbsp;</TD><TD><INPUT NAME="fullname" TYPE="text" id="fullname" /></TD></TR>
		<TR><TD><label for="email">Enter your email address*:&nbsp;</label></TD><TD>&nbsp;</TD><TD><INPUT NAME="email" TYPE="text" id="email" /></TD></TR>
		<TR><TD>&nbsp;</TD><TD>&nbsp;</TD><TD>&nbsp;</TD></TR>
		<TR><TD>&nbsp;</TD><TD><DIV class="submit"><INPUT type="submit" value="Submit" /></TD><TD>&nbsp;</TD></TR>
		</TABLE>
		</FORM><BR><span class="tiny">&nbsp;&nbsp;* Required field&nbsp;</span><BR>' ;
}

// registration logic handling
function register(){
	global $db ;
	list($auth_check0,$username)=auth_check() ;
	if($auth_check0 && strlen($username)){
		// already logged in (cookie exists)
		echo 'You are already logged in (and registered) as <B>' . $username . '</B>.<BR><BR>Please <A HREF="login.php">logout</a> if you wish to register a new account.<BR><BR>THANKS!' ;
	}else{
		if(isset($_POST['username']) && strlen(trim($_POST['username']))>4){
			$username=strtolower(trim($_POST['username'])) ;
			// password check
			if(isset($_POST['password0']) && isset($_POST['password1']) && strlen(trim($_POST['password0']))>5 && strlen(trim($_POST['password1']))>5){
				$password0=trim($_POST['password0']);
				$password1=trim($_POST['password1']);
				if($password0==$password1){
					$user_exits_sql="SELECT count(username) FROM users WHERE username='$username'" ;
					$user_exits_query=pg_query($db,$user_exits_sql) ;
					$user_exits=pg_result($user_exits_query,0,0) ;
					if($user_exits>0){
						echo '<span class="bad">' . $username . ' is already a registered user, please choose a different username.</span><BR><BR>' ;
						register_form();
					}else{
						$name=trim($_POST['fullname']);
						$email=trim($_POST['email']);
						$sha1_password=sha1($password0) ;
						$add_user_sql="INSERT INTO users (username,password,name,email) VALUES ('$username','$sha1_password','$name','$email')" ;
						$add_user_query=pg_query($db,$add_user_sql) ;
						if(!$add_user_query){
							echo '<span class="bad">Username creation FAILED.</span><BR><BR>' ;
						}else{
							echo 'Your account has been successfully created.  You may now <A HREF="login.php?do=0">login</a>.<BR><BR>' ;
						}
					}
				}else{
					echo '<span class="bad">Passwords do not match.  Please try again.</span><BR>' ;
					register_form();
				}
			}else{
				echo '<span class="bad">Passwords are not at least 6 characters long.  Please try again.</span><BR>' ;
				register_form();
			}
		}else{
			register_form();
		}		
	}
}

// verify hash cookie is valid for this user
function hash_check(){
	global $db ;
	if(isset($_COOKIE['tasti_username'])){
		$username=$_COOKIE['tasti_username'] ;
	}else{
		$username='' ;
	}
	if(isset($_COOKIE['tasti_hash'])){
		$password_hash=$_COOKIE['tasti_hash'] ;
	}else{
		$password_hash='' ;
	}
	
	$hash_sql="SELECT count(id) FROM users WHERE username='$username' AND password='$password_hash'" ;
	$hash_qry=pg_query($db,$hash_sql) ;
	if(!$hash_qry){
		return 0 ;
	}else{
		return pg_result($hash_qry,0,0) ;
	}
}

// verify login and create cookie
function login_check(){
	global $db ;
	if(isset($_POST['username']) && isset($_POST['password']) && strlen(trim($_POST['password'])) && strlen(trim($_POST['username']))){
		$username=strtolower(trim($_POST['username'])) ;
		$password=sha1(trim($_POST['password'])) ;
		$isAuthenticated_sql="SELECT count(id) FROM users WHERE username='$username' AND password='$password'" ;
		$isAuthenticated_query=pg_query($db,$isAuthenticated_sql) ;
		if(!$isAuthenticated_query){
			echo '<span class="bad">CANNOT CONNECT TO DATABASE</span>' ;
			exit ;
		}else{
			// 0=auth failed 	1=auth successful
			$isAuthenticated=pg_result($isAuthenticated_query,0,0) ;
			if($isAuthenticated=='1'){
				$expire=time()+(3600*24*LOGIN_AGE) ;
				setcookie("tasti_username",$username,$expire) ;
				setcookie("tasti_hash",$password,$expire) ;
				return 1 ;
			}else{
				$expire=time()-100 ;
				setcookie("tasti_username",$username,$expire) ;
				setcookie("tasti_hash",$password,$expire) ;
				return 0 ;
			}
		}
	}elseif(isset($_COOKIE['tasti_username']) && isset($_COOKIE['tasti_hash']) && isset($_GET['do']) && $_GET['do']==1){
		$username=$_COOKIE['tasti_username'];
		$password=$_COOKIE['tasti_hash'] ;
		$expire=time()-100 ;
		setcookie("tasti_username",$username,$expire) ;
		setcookie("tasti_hash",$password,$expire) ;
		return 3 ;
	}else{
		return 2 ;
	}
}

// login form
function login_form(){
	echo '<BR><span class="huge">Enter your <B>Tasti</B> username and password to login</span><BR><BR>' ;
	echo '<FORM method="POST" action="login.php" id="login">
		<TABLE>
		<TR><TD>Username &nbsp;</TD><TD>&nbsp;</TD><TD><INPUT TYPE="text" NAME="username" id="username" /></TD></TR>
		<TR><TD>Password &nbsp;</TD><TD>&nbsp;</TD><TD><INPUT TYPE="password" NAME="password" id="password" /></TD></TR>
		<TR><TD>&nbsp;</TD><TD>&nbsp;</TD><TD>&nbsp;</TD></TR>
                <TR><TD>&nbsp;</TD><TD><DIV class="submit"><INPUT type="submit" value="Submit" /></TD><TD>&nbsp;</TD></TR>
		</TABLE></FORM><BR><BR>' ;
}

// handle login or logout logic
function login($isAuthenticated){
	list($auth_check0,$username)=auth_check() ;
	if(($auth_check0=='1' && strlen($username)>='0' && $isAuthenticated=='1') || $isAuthenticated=='1'){
		echo '<BR>Congratulations, you have successfully logged in.<BR><BR>' ;
		show_bmarks() ;
	}elseif($isAuthenticated=='3'){
		echo '<BR>You have logged out.<BR><BR>' ;
		login_form() ;	
	}elseif($isAuthenticated=='0'){
		echo '<BR><span class="bad">Invalid username or password.  Please try to login again.</span><BR><BR>';
		login_form() ;	
	}else{
		login_form() ;	
	}
}

?>
