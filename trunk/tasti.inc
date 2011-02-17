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
	echo '<span class="footer_class1"><CENTER><A HREF="index.php">HOME</a>&nbsp;|&nbsp;<A HREF="mailto:' . ADMIN_EMAIL . '">HELP</a><BR><A HREF="https://code.google.com/p/tasti/">Tasti</a> is licensed under the <A HREF="http://www.gnu.org/licenses/gpl.html">GPL</a>.  Copyright ' . $year . '<BR></CENTER></span>' ;
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
                        			$tagname=htmlentities(trim(pg_result($all_tags_qry,$tagid,0)), ENT_QUOTES,"UTF-8") ;
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
					$tag=htmlentities($tag,ENT_QUOTES, "UTF-8") ;
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
		$bmark_notes=htmlentities($bmark_notes,ENT_QUOTES, "UTF-8") ;
		$bmark_name=htmlentities($bmark_name,ENT_QUOTES, "UTF-8") ;
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
	if(isset($_COOKIE['tasti_bmarks_per_page'])){
		$bmarks_per_page=$_COOKIE['tasti_bmarks_per_page'] ; 
	}else{
		$bmarks_per_page=15 ;
	}
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
		$expire=time()+(3600*24*LOGIN_AGE) ;
		setcookie("tasti_bmarks_per_page",$user_num_bmarks,$expire) ;
	}else{
		$user_num_bmarks=$bmarks_per_page ;
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
	}
	// pagination setup
	$page=(!isset($_GET['page']))? 1 : $_GET['page'] ;  
        $prev=($page - 1) ;
        $next=($page + 1) ; 
        $max_results=$user_num_bmarks ; 
	// Calculate the offset 
        $from=(($page * $max_results) - $max_results) ;
	
	if(isset($_GET['whose']) && $_GET['whose']=='mine' && hash_check()){
		$username=$_COOKIE['tasti_username'] ;
		$whose='mine' ;
		$url_get_base=basename($_SERVER['REQUEST_URI']) . '&' ;
		$num_bmarks_menu_offset='77' ;
		$bmarks_intro='<span class="huge">Your bookmarks:</span><BR><BR>' ;
		$bmarks_sql_all="SELECT distinct ON (url) url,id,date(last_update) AS last_update,notes,name,owner FROM bmarks WHERE owner='$username' ORDER BY	url,last_update" ;
	}elseif(isset($_GET['whose']) && $_GET['whose']!='mine'){
		$owner0=$_GET['whose'] ;
		$whose=$owner0 ;
		$url_get_base=basename($_SERVER['REQUEST_URI']) . '&' ;
		$num_bmarks_menu_offset='177' ;
		$bmarks_intro='<span class="huge">' . $owner0 . '\'s&nbsp;bookmarks:</span><BR><BR>' ;
		$bmarks_sql_all="SELECT distinct ON (url) url,id,date(last_update) AS last_update,notes,name,owner FROM bmarks WHERE owner='$owner0' ORDER BY url,last_update" ;
	}else{
		$num_bmarks_menu_offset='73' ;
		$whose='' ;
		$url_get_base=basename($_SERVER['REQUEST_URI']) . '?' ;
		$bmarks_intro='<span class="huge">Recent bookmarks:</span><BR><BR>' ;
		$bmarks_sql_all="SELECT distinct ON (url) url,id,date(last_update) AS last_update,notes,name,owner FROM bmarks ORDER BY url,last_update" ;
	}
	$bmarks_sql=$bmarks_sql_all . $limit_sql . ' OFFSET ' . $from ;
	$bmarks_qry=pg_query($db,$bmarks_sql) ;
	if(!$bmarks_qry){
		echo '<span class="bad">Bookmark query FAILED: <BR>' ;
                echo pg_last_error() . '</span><BR><BR>';
                exit() ;
	}else{
		$bmarks_qry_all=pg_query($db,$bmarks_sql_all) ;
		$num_bmarks_all=pg_num_rows($bmarks_qry_all) ;
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
				$notes=str_replace('&amp;quot;','"',str_replace('&amp;#039;',"'",htmlspecialchars_decode(htmlentities(pg_result($bmarks_qry,$lt,3))))) ;
				$name=str_replace('&amp;quot;','"',str_replace('&amp;#039;',"'",htmlspecialchars_decode(htmlentities(pg_result($bmarks_qry,$lt,4))))) ;
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
						$bmark_user_edit_string='<BR><span class="normal">Created by <A HREF="bmarks.php?whose=' . $owner . '"><B>' . $owner . '</B></a>&nbsp;</span>' ;
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
				}
			}

			// pagination settings
			$my_row_count=$num_bmarks_all ;
			$total_pages=ceil($my_row_count / $max_results) ;
			$pagination = '' ; 

			// Create a PREV link if one is needed 
                        if($page > 1) {
				$pagination.='<A STYLE="text-decoration:none" title="PREVIOUS PAGE" HREF="bmarks.php?whose=' . $whose . '&page=' . $prev . '&num=' . $user_num_bmarks;
				$pagination .= '"><span class="huge"><H1>&larr;</H1></span></A> ' ;
			}

			if(!strlen($whose)){
				$page=0 ;
				$total_pages=0 ;
			}
			// Create a NEXT link if one is needed
                        if($page < $total_pages){
				$pagination .= '<A STYLE="text-decoration:none" title="NEXT PAGE" HREF="bmarks.php?whose=' . $whose . '&page=' . $next . '&num=' . $user_num_bmarks;  ;
				$pagination .= '"><span class="huge"><H1>&rarr;</H1></span></A>' ;
			}else{
                                // adjust the total count when on the last page since it might not have $user_num_bmarks items remaining
                                $new_max_results = ($max_results - (($page * $max_results) - $my_row_count)) ;
                                $max_results=$new_max_results ;
                        }
			if($my_row_count>1){
				$plural='s' ;
			}else{
				$plural='' ;
			}

			echo '<TABLE width="100%"><TR COLSPAN="1"><TD>&nbsp;</TD></TR><TR><TD COLSPAN="9">&nbsp;' . $my_row_count . ' Tasti bookmark' . $plural . '</TD><TD
			class="page" COLSPAN="4"><B>' . $pagination . '</B></TD></TR></TABLE><BR>' ;
		}else{
			echo '<BR>There are currently no bookmarks<BR><BR>' ;
		}
	}
}

// show tags
function show_tags(){
	global $db ;
	$num_bmarks_menu_offset='' ;
	$show_mine='' ;
	$tag_get='' ;
	
	if(isset($_GET['id'])){
		$tagid=$_GET['id'] ;
		$tag_get='&id=' . $tagid ;
	}

	$tags_sql="SELECT tag FROM tags " ;

	if(isset($_COOKIE['tasti_username']) && hash_check()){
		$username=$_COOKIE['tasti_username'] ;
		if(isset($_GET['mine']) && $_GET['mine']=='yes'){
			$show_mine="AND owner='$username'" ;
			$url_get_base=basename($_SERVER['REQUEST_URI']) . '&' ;
			$num_bmarks_menu_offset='47' ;
		}
		if(isset($tagid)){
			$tagid=$_GET['id'] ;
			$tags_sql.="WHERE id='$tagid' " . $show_mine . " LIMIT 1" ;
		}else{
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
			if(isset($_COOKIE['tasti_bmarks_per_page'])){
				$bmarks_per_page=$_COOKIE['tasti_bmarks_per_page'] ; 
			}else{
				$bmarks_per_page=15 ;
			}
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
				$expire=time()+(3600*24*LOGIN_AGE) ;
				setcookie("tasti_bmarks_per_page",$user_num_bmarks,$expire) ;
			}else{
				$user_num_bmarks=$bmarks_per_page ;
				$limit_sql=' LIMIT ' . $user_num_bmarks;
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
			}
			// pagination setup
			$page=(!isset($_GET['page']))? 1 : $_GET['page'] ;  
        		$prev=($page - 1) ;
        		$next=($page + 1) ; 
        		$max_results=$user_num_bmarks ; 
			// Calculate the offset 
        		$from=(($page * $max_results) - $max_results) ;
			
			$tag=htmlentities(pg_result($tags_qry,0,0)) ;
			$bmark_sql="SELECT id,date(last_update) as last_update,owner,url,notes,name FROM bmarks WHERE tag='$tag' " ;
			if(isset($username) && isset($_GET['mine']) && $_GET['mine']=='yes'){
				$bmark_sql.=" AND owner='$username' " ;
				$mine='&mine=yes' ;
			}else{
				$mine='' ;
			}
			$bmarks_sql_all=$bmark_sql . 'ORDER BY owner,last_update,name' ;
			$bmark_sql=$bmarks_sql_all . $limit_sql . ' OFFSET ' . $from ;
			$bmark_qry=pg_query($db,$bmark_sql) ;
			if(!$bmark_qry){
				echo '<span class="bad">Bookmark query FAILED: <BR>' ;
                		echo pg_last_error() . '</span><BR><BR>';
			}else{
				$bmarks_qry_all=pg_query($db,$bmarks_sql_all) ;
				$num_bmarks_all=pg_num_rows($bmarks_qry_all) ;
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
						$notes=str_replace('&amp;quot;','"',str_replace('&amp;#039;',"'",htmlentities(pg_result($bmark_qry,$lt,4)))) ;
						$name=str_replace('&amp;quot;','"',str_replace('&amp;#039;',"'",htmlentities(pg_result($bmark_qry,$lt,5)))) ;
					
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
					// pagination settings
					$my_row_count=$num_bmarks_all ;
					$total_pages=ceil($my_row_count / $max_results) ;
					$pagination = '' ; 

					// Create a PREV link if one is needed 
                        		if($page > 1) {
						$pagination.='<A STYLE="text-decoration:none" title="PREVIOUS PAGE" HREF="tags.php?page=' . $prev . '&num=' . $user_num_bmarks . $mine . $tag_get;
						$pagination .= '"><span class="huge"><H1>&larr;</H1></span></A> ' ;
					}

					// Create a NEXT link if one is needed
                        		if($page < $total_pages){
						$pagination .= '<A STYLE="text-decoration:none" title="NEXT PAGE" HREF="tags.php?page=' . $next . '&num=' . $user_num_bmarks . $mine . $tag_get; 
						$pagination .= '"><span class="huge"><H1>&rarr;</H1></span></A>' ;
					}else{
                                		// adjust the total count when on the last page since it might not have $user_num_bmarks items remaining
                                		$new_max_results = ($max_results - (($page * $max_results) - $my_row_count)) ;
                                		$max_results=$new_max_results ;
                        		}
					if($my_row_count>1){
						$plural='s' ;
					}else{
						$plural='' ;
					}

					echo '<TABLE width="100%"><TR COLSPAN="1"><TD>&nbsp;</TD></TR><TR><TD COLSPAN="9">&nbsp;' . $my_row_count . ' Tasti bookmark' . $plural . '</TD><TD
					class="page" COLSPAN="4"><B>' . $pagination . '</B></TD></TR></TABLE><BR>' ;
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
						$tag=htmlentities($tag,ENT_QUOTES,"UTF-8") ;
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
			$bmark_name=htmlentities($bmark_name,ENT_QUOTES,"UTF-8") ;
			$bmark_notes=htmlentities($bmark_notes,ENT_QUOTES,"UTF-8") ;
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
	$name='' ;
	$url='' ;
	
	if(isset($_GET['url'])){
		$url=$_GET['url'] ;
	}
	if(isset($_GET['name'])){
		$name=$_GET['name'] ;
	}
	
	$username=$_COOKIE['tasti_username'] ;
	$tags_sql="SELECT tag FROM tags WHERE owner='$username' ORDER BY tag LIMIT 100" ;
	$tags_query=pg_query($db,$tags_sql) ;
	$num_users_tags=pg_num_rows($tags_query) ;
	
	echo '<span class="huge">Add a new bookmark to <B>Tasti</B>:</span><BR><BR>' ;
	echo '<FORM method="POST" action="add.php" id="add_bmark"><TABLE>
		<TR><TD>Name/Description*:&nbsp;</TD><TD>&nbsp;</TD><TD><INPUT TYPE="text" NAME="name" id="name" size="55" value="' . $name . '"></TD></TR>
		<TR><TD>URL*:&nbsp;</TD><TD>&nbsp;</TD><TD><INPUT TYPE="text" NAME="url" id="url" size="55" value="' . $url . '"></TD></TR>
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

// bookmark import form
function bmark_import_form (){
	echo '<div id="content"><span class="big"><B>Tasti</B> currently supports bulk imports via a bookmarks file (either from your web browser, or from a <A HREF="http://www.delicious.com">Delicious</a> export):</span><BR><BR>' ;
	echo '<FORM method="POST" action="import.php" id="import" enctype="multipart/form-data"><UL>' ;
	echo '<LI>&nbsp;Upload a bookmarks file from your web browser:&nbsp;<INPUT TYPE="file" NAME="upload_bmark"><BR>&nbsp;</LI>
		<LI>&nbsp;<input type="checkbox" name="import_tag">&nbsp;&nbsp;Add the \'imported\' tag to each bookmark<BR>&nbsp;</LI> 
		</UL><BR><CENTER><INPUT TYPE="submit" NAME="SAVE" value="Import"></CENTER></FORM><BR></div>' ;
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
	if($pagename=='bmarklet.php'){
		$bmarkletSelected='id="selected"' ;
	}else{
		$bmarkletSelected='' ;
	}
	if($pagename=='edit_tags.php'){
		$tagSelected='id="selected"' ;
	}else{
		$tagSelected='' ;
	}
	echo '<div id="tabs"><ul>
		<li ' . $accountSelected . '><a href="account.php">Details</a></li><!-- these comments between lis solve a bug in IE that prevents spaces appearing between list items that appear on different lines in the source
			--><li ' . $importSelected . '><a href="import.php">Import&nbsp;Bookmarks</a></li><!-- these comments between lis solve a bug in IE that prevents spaces appearing between list items that appear on different lines in the source
			--><li ' . $bmarkletSelected . '><a href="bmarklet.php">Bookmarklet</a></li><!-- these comments between lis solve a bug in IE that prevents spaces appearing between list items that appear on different lines in the source
			--><li ' . $tagSelected . '><a href="edit_tags.php">Tags</a></li><!-- ' ;
		echo '--></ul>
		</div>' ;
	return $pagename ;
}

// process the bookmark file for import
function bmark_import_file(){
	
	$bmark_array=file(($_FILES['upload_bmark']['tmp_name'])) ;
	$imported_bmark_count=0 ;
	$importBad=0 ;
	foreach($bmark_array as $bmark_row){
		if(preg_match("/href/i",$bmark_row) && preg_match("/http/i",$bmark_row)){
			$row_parts=explode('href',strtolower($bmark_row),2) ;
			foreach($row_parts as $row_part){
				if(preg_match("/http/i",$row_part)){
					$url_array=explode('"',trim($row_part)) ;
					$item_count=count($url_array) - 1 ;
					$url=$url_array['1'] ;
					$bmark_name=str_replace('</a>','',preg_replace('/^>/','',$url_array[$item_count])) ;

					// process tags, if they exist
					if(preg_match('/tags="/',$row_part)){
						$tag_array=explode('tags="',trim($row_part)) ;
						$tags0=explode(',',trim(current(explode('"',$tag_array[1])))) ;
						$tags=array_filter($tags0) ;
					}else{
						$tags=array() ;
					}
					// add the imported tag
					if(isset($_POST['import_tag'])){
						$tags[]='imported' ;
					}
					$tags=array_unique($tags) ;
					$username=$_COOKIE['tasti_username'] ;
					// drop javascript URLs
					if(!preg_match('/^javascript:/',$url)){
						$insertRC=insert_bmark($username,$url,$bmark_name,$tags) ;
						if($insertRC){
							$importBad++ ;
						}else{
							$imported_bmark_count++ ;
						}
					}
				}
			}
		}
	}
	
	echo '<div id="content">' ;
		if($imported_bmark_count > 0){
			echo '<BR>&nbsp;<B>SUCCESS!</B><BR><BR>You have imported ' . $imported_bmark_count . ' bookmarks.<BR><BR>Click <A HREF="bmarks.php?whose=mine">here</a> to view them now.<BR><BR>' ;
			if($importBad>0){
				echo $importBad . ' bookmark(s) failed to import.<BR><BR>' ;
			}
		}else{
			echo '<BR>&nbsp;No bookmarks were successfully imported.  Please verify that your bookmarks file is valid, and then try again later.<BR><BR>' ;
		}
	echo '</div>' ; 
}

// insert a new bookmark into the database 
function insert_bmark ($username,$bmark_url,$bmark_name,$bmark_tags_array) {
	global $db ;
	$bmark_notes='' ;
	
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
				pg_close($db) ;
				exit() ;
			}else{
				$tag_count=pg_result($userHasTag_query,0,0) ;
				if($tag_count==0){
					// if the tag doesn't exist, insert it
					if(strlen($tag)){
						$tag=htmlentities($tag,ENT_QUOTES) ;
						$addUserTag_sql="INSERT INTO tags (owner,tag) VALUES ('$username','$tag')" ;
						$addUserTag_qry=pg_query($db,$addUserTag_sql) ;
						if(!$addUserTag_qry){
							echo '<span class="bad">Tag INSERT FAILED: <BR>' ;
							echo pg_last_error() . '</span><BR><BR>';
							$tr_end = pg_query($db,'ROLLBACK;');
							pg_close($db) ;
							exit() ;
						}
					}
				}
			}
			$check_dup_sql="SELECT count(id) FROM bmarks WHERE owner='$username' AND url='$bmark_url' AND tag='$tag'" ;
			$check_dup_qry=pg_query($db,$check_dup_sql) ;
			$dup_count=pg_result($check_dup_qry,0,0) ;
			if($dup_count==0){
				$bmark_name=htmlentities($bmark_name,ENT_QUOTES,"UTF-8") ;
				$bmark_notes=htmlentities($bmark_notes,ENT_QUOTES,"UTF-8") ;
				$addBmark_sql="INSERT INTO bmarks (owner,url,notes,tag,name) VALUES ('$username','$bmark_url','$bmark_notes','$tag','$bmark_name')" ;
				$addBmark_qry=pg_query($db,$addBmark_sql) ;
				if(!$addBmark_qry){
					echo '<span class="bad">Bookmark INSERT FAILED: <BR>' ;
					echo pg_last_error() . '</span><BR><BR>';
					$tr_end = pg_query($db,'ROLLBACK;');
					pg_close($db) ;
					exit() ;
				}
			}
		}
		
		// rollback everything if any part of the transaction failed
		if($tr_status){
			$tr_end = pg_query($db,'ROLLBACK;');
			pg_close($db) ;
			exit();
		}
	$tr_end=pg_query($db,'COMMIT;');
	return 0 ;
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
			if(isset($_FILES['upload_bmark'])){
				bmark_import_file() ;
			}else{
				bmark_import_form() ;
			}
		}elseif($pagename=='bmarklet.php'){
			show_Bmarklet() ;
		}elseif($pagename=='edit_tags.php'){
			edit_tags() ;
		}else{
			echo '<BR>Unknown function<BR><BR>' ;
		}
	}else{
		echo '<BR>This page is only accessible to users who have <A HREF="login.php?do=0">logged in</a>.<BR><BR>' ;
	}	
}

// delete tags
function delete_tags ($taglist) {
	global $db ;
	$username=$_COOKIE['tasti_username'] ;
	
	$tr_status=0 ;
	$tr_begin=pg_query($db,'BEGIN;');
	$tag_count=count($taglist) ;
	foreach($taglist AS $tagid){
		// get the tag name 
		$tag_name_sql="SELECT tag FROM tags WHERE id='$tagid' AND owner='$username'" ;
		$tag_name_qry=pg_query($db,$tag_name_sql) ;
		if(!$tag_name_qry){
			$tr_end = pg_query($db,'ROLLBACK;');
			echo '<span class="bad">Tag name query FAILED<BR>' ;
			echo pg_last_error() . '</span><BR><BR>';
			$tr_status=1 ;
			break ;
		}
		$tag_name=pg_result($tag_name_qry,0,0) ;
		
		// remove the tag from any associated bookmarks
		$remove_tag_from_bmark_sql="UPDATE bmarks SET tag=null WHERE owner='$username' AND tag='$tag_name'" ;
		$remove_tag_from_bmark_qry=pg_query($db,$remove_tag_from_bmark_sql) ;
		if(!$remove_tag_from_bmark_qry){
			$tr_end = pg_query($db,'ROLLBACK;');
			echo '<span class="bad">Tag update FAILED<BR>' ;
			echo pg_last_error() . '</span><BR><BR>';
			$tr_status=1 ;
			break ;
		}
		
		// delete the tag
		$tag_del_sql="DELETE FROM tags WHERE owner='$username' AND id='$tagid'" ;
		$tag_del_qry=pg_query($db,$tag_del_sql) ;
		if(!$tag_del_qry){
			$tr_end = pg_query($db,'ROLLBACK;');
			echo '<span class="bad">Tag deletion FAILED<BR>' ;
			echo pg_last_error() . '</span><BR><BR>';
			$tr_status=1 ;
			break ;
		}
	}
	// commit as long as nothing above failed
	if(!$tr_status){
		$tr_end=pg_query($db,'COMMIT;');
		if($tag_count>1){
			$pl='s' ;
		}else{
			$pl='' ;
		}
		echo '<BR>&nbsp;<span class="huge">' . $tag_count . ' tag' . $pl . ' deleted successfully.</span><BR><BR><BR>' ;
	}
}

// rename tags
function tag_rename($tagname) {
	global $db ;
	$username=$_COOKIE['tasti_username'] ;
	$tr_status=0 ;
	$tr_begin=pg_query($db,'BEGIN;');
	
	foreach($tagname as $id=>$tag){
		$new_tag_name=trim(htmlentities($tag, ENT_QUOTES,'UTF-8')) ;
		// only rename if the new tag name is at least 1 char long
		if(strlen($new_tag_name)){
			$old_tag_name_sql="SELECT tag FROM tags WHERE owner='$username' AND id='$id' LIMIT 1" ;
			$old_tag_name_qry=pg_query($db,$old_tag_name_sql) ;
			if(!$old_tag_name_qry){
				$tr_end = pg_query($db,'ROLLBACK;');
				echo '<span class="bad">Tag name query FAILED<BR>' ;
				echo pg_last_error() . '</span><BR><BR>';
				$tr_status=1 ;
				break ;
			}
			$old_tag_name=pg_result($old_tag_name_qry,0,0) ;
		
			$bmark_tag_rename_sql="UPDATE bmarks SET tag='$new_tag_name' WHERE owner='$username' AND tag='$old_tag_name'" ;
			$bmark_tag_rename_qry=pg_query($db,$bmark_tag_rename_sql) ;
			if(!$bmark_tag_rename_qry){
				$tr_end = pg_query($db,'ROLLBACK;');
				echo '<span class="bad">Tag bookmark rename query FAILED<BR>' ;
				echo pg_last_error() . '</span><BR><BR>';
				$tr_status=1 ;
				break ;
			}
			
			$tag_rename_sql="UPDATE tags SET tag='$new_tag_name',last_update=now() WHERE owner='$username' AND id='$id'" ;
			$tag_rename_qry=pg_query($db,$tag_rename_sql) ;
			if(!$tag_rename_qry){
				$tr_end = pg_query($db,'ROLLBACK;');
				echo '<span class="bad">Tag rename query FAILED<BR>' ;
				echo pg_last_error() . '</span><BR><BR>';
				$tr_status=1 ;
				break ;
			}
		}
	}
	
	// commit as long as nothing above failed
	if(!$tr_status){
		$tr_end=pg_query($db,'COMMIT;');
		echo '<BR>&nbsp;<span class="huge">Tags successfully renamed.</span><BR><BR><BR>' ;
	}
}

// manage edit/delete of tags
function edit_tags () {
	global $db ;
	$username=$_COOKIE['tasti_username'] ;
	
	if(isset($_POST['submit']) && isset($_POST['taglist'])){
		$opt_type=$_POST['submit'] ;
		if($opt_type=="DELETE"){
			delete_tags($_POST['taglist']) ;
		}
	}elseif(isset($_POST['submit'])){
		$opt_type=$_POST['submit'] ;
		if($opt_type=="RENAME"){
			tag_rename($_POST['tagname']) ;
		}
	}
	
	$tag_list_sql="SELECT id,tag FROM tags WHERE owner='$username' ORDER BY tag" ;
	$tag_list_qry=pg_query($db,$tag_list_sql) ;
	if(!$tag_list_qry){
		echo '<BR><span class="bad">Tag query failed.<BR><BR>' ;
		echo pg_last_error() . '</span><BR><BR>';
		exit() ;
	}
	$tag_count=pg_num_rows($tag_list_qry) ;
	if($tag_count>0){
		echo '<BR><span class="huge">Edit the tags that you wish to rename, or check the tags that you want to delete:</span>' ;
		echo '<FORM method="POST" action="edit_tags.php" id="edit_tags">' ;
		echo '<span class="normal">&nbsp;Toggle all&nbsp;</span><INPUT TYPE="checkbox" onclick="toggle(\'chkbox1\')"><BR><BR>' ;
		echo '<CENTER><div id="chkbox1"><TABLE><TR>' ;
		
		$row_count=0 ;
		for($lt=0 ; $lt <$tag_count ; $lt++){
			$id=pg_result($tag_list_qry,$lt,0) ;
			$tag=pg_result($tag_list_qry,$lt,1) ;
			
			echo '<TD><INPUT type="checkbox" id="' . $id . '"  name="taglist[]" value="' . $id . '" >&nbsp;<span class="big"><INPUT TYPE="text" NAME="tagname[' . $id . ']" id="tag" size="15" VALUE="' . $tag . '"></span>&nbsp;&nbsp;&nbsp;<BR>&nbsp;</TD>' ;
			
			$row_count++ ;
			if($row_count>4){
				echo '</TR><TR>' ;
				$row_count=0 ;
			}
		}
		echo '</TR></TABLE></DIV><INPUT type="submit" name="submit" value="DELETE" />&nbsp;&nbsp;<INPUT name="submit" type="submit" value="RENAME" />' ;
		
		echo '</CENTER></FORM><BR><BR>' ;
	}else{
		echo '<BR><span class="huge">&nbsp;You do not have any tags to edit at this time.  Try <A HREF="add.php">adding</a> a bookmark to create new
		tags</span><BR><BR><BR>' ;
	}
}

// show the add bookmark bookmarklet link
function show_Bmarklet () {
	$add_bmark_url=dirname($_SERVER['SCRIPT_URI']) ;
	$bmarklet_string="javascript:(function(){f='" . $add_bmark_url . "/add.php?url='+encodeURIComponent(window.location.href)+'&name='+encodeURIComponent(document.title)+'&notes='+encodeURIComponent(''+(window.getSelection?window.getSelection():document.getSelection?document.getSelection():document.selection.createRange().text));a=function(){if(!window.open(f+'noui=1&jump=doclose','d','location=yes,links=no,scrollbars=no,toolbar=no,width=550,height=550'))location.href=f+'jump=yes'};if(/Firefox/.test(navigator.userAgent)){setTimeout(a,0)}else{a()}})()";
	
	echo '<BR><span class="huge">&nbsp;A bookmarklet is a link that you add to your browser\'s Toolbar. It makes it easy to add a new bookmark to <B>Tasti</B>.</span><BR><BR>
		Drag the link below to your toolbar, and then you can click that link when viewing any web page to quickly & easily add it as a bookmark.<BR><BR><BR>
		<CENTER><A HREF="' . $bmarklet_string . '"><B>Add to Tasti</B></a></CENTER><BR><BR><BR>' ;
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
	if(isset($_COOKIE['tasti_bmarks_per_page'])){
		$bmarks_per_page=$_COOKIE['tasti_bmarks_per_page'] ; 
	}else{
		$bmarks_per_page=10 ;
	}
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
				setcookie("tasti_bmarks_per_page",$bmarks_per_page,$expire) ;
				return 1 ;
			}else{
				$expire=time()-100 ;
				setcookie("tasti_username",$username,$expire) ;
				setcookie("tasti_hash",$password,$expire) ;
				setcookie("tasti_bmarks_per_page",$bmarks_per_page,$expire) ;
				return 0 ;
			}
		}
	}elseif(isset($_COOKIE['tasti_username']) && isset($_COOKIE['tasti_hash']) && isset($_GET['do']) && $_GET['do']==1){
		$username=$_COOKIE['tasti_username'];
		$password=$_COOKIE['tasti_hash'] ;
		$expire=time()-100 ;
		setcookie("tasti_username",$username,$expire) ;
		setcookie("tasti_hash",$password,$expire) ;
		setcookie("tasti_bmarks_per_page",$bmarks_per_page,$expire) ;
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
