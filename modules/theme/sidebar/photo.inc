<?php
/**Enables 
*<ul>
*<li>displaying/viewing of photos</li>
*<li>editing of photos</li>
*<li>linking to relevant locations for further operations </li>
*<li>caching of most likely viewed pictures</li>
* if ($photoNext)
 * {
 *   echo '<img style="display:none" src="'.$photoNext->GetMedia('scaled').'" alt="hidden photo">';
 * }
*</ul>
*<b>Note </b>$cameralife and $photo are set by ShowPage()
* <code>
*if (!$cameralife->Security->GetName())
   * {
   *   $condition = "user_ip='".$cameralife->Security->GetAddr()."' AND user_name=''";
  *    $count_edits = $cameralife->Database->SelectOne('logs','COUNT(*)',$condition);
  *    if ($count_edits % 3 == 0)
    *    $ask_user_to_login = true;
    *}
*</code>The above code prompts if user is doing useful edits prompts them to log in or register 
 
 * <code>
 *if ($photoNext)
 * {
 *   echo '<img style="display:none" src="'.$photoNext->GetMedia('scaled').'" alt="hidden photo">';
 * }
 *</code>
 *Caches the next image the user is likely to look at
  *@author Will Entriken <cameralife@phor.net>
  *@copyright Copyright (c) 2001-2009 Will Entriken
  *@access public
  */
  /**
  */
  # $cameralife and $photo are set by ShowPage()

  $cameralife->GetFeature('security');
  $cameralife->GetFeature('photostore');
  $cameralife->GetFeature('imageprocessing');

  # Display a photo, edit a photo, link to relevant places

  if ($_GET['referer'])
    $_SERVER['HTTP_REFERER'] = $_GET['referer'];
  $_SERVER['HTTP_REFERER'] = str_replace($cameralife->base_url,'', $_SERVER['HTTP_REFERER']);
  $_SERVER['HTTP_REFERER'] = preg_replace('|^/|', '', $_SERVER['HTTP_REFERER']);
  if (preg_match('|^photos/|',$_SERVER['HTTP_REFERER']))
    $_SERVER['HTTP_REFERER'] = ''; // not useful information

  $sort = $_COOKIE["sort"] or $sort = 'id desc';

  if ($photo->Get('status') != 0) 
    $cameralife->Security->authorize('admin_file', 'This file has been flagged or marked private');
  $icon = $photo->GetIcon('small');
  if (strpos($icon['href'], '&#63;') !== FALSE)
    $hrefinquery = $icon['href'] . '&amp;';
  else
    $hrefinquery = $icon['href'] . '&#63;';

  if ($cameralife->Security->GetName())
    $rating = $avg = $cameralife->Database->SelectOne('ratings', 'AVG(rating)', 'id='.$photo->Get('id')." AND username='".$cameralife->Security->GetName()."'");
  else
    $rating = $avg = $cameralife->Database->SelectOne('ratings', 'AVG(rating)', 'id='.$photo->Get('id')." AND user_ip='".$_SERVER['REMOTE_ADDR']."'");

/*    // If the user is not logged in and is making helpful edits, invite them to register / login
    if (!$cameralife->Security->GetName())
    {
      $condition = "user_ip='".$cameralife->Security->GetAddr()."' AND user_name=''";
      $count_edits = $cameralife->Database->SelectOne('logs','COUNT(*)',$condition);
      if ($count_edits % 3 == 0)
        $ask_user_to_login = true;
    }
*/

  $menu = array();
  $menu[] = $cameralife->GetIcon('small');

  // Show all the tasks the user can do (if they're allowed)
  if ($cameralife->Security->authorize('photo_rename'))
  {
    $menu[] = array('name'=>'Rename photo',
                    'href'=>$hrefinquery . 'action=rename&amp;referer='.urlencode($_SERVER['HTTP_REFERER']),
                    'image'=>'small-admin',
                    'section'=>'Tasks',
                    'onclick'=>"return toggleshowrename();");
  }
  if ($cameralife->Security->authorize('photo_delete'))
  {
    $menu[] = array('name'=>'Flag / Report photo',
                    'href'=>$hreyinquery . "id=".$photo->Get('id')."&amp;referer=".urlencode($_SERVER['HTTP_REFERER'])."&amp;action=delete",
                    'image'=>'small-admin',
                    'section'=>'Tasks',
                    'onclick'=>"document.getElementById('delete').style.display='';return false");
  }
  if ($cameralife->Security->authorize('photo_modify'))
  {
    $menu[] = array('name'=>'Modify (rotate, revert, ...)',
                    'href'=>$hrefinquery . "id=".$photo->Get('id')."&amp;referer=".urlencode($_SERVER['HTTP_REFERER'])."&amp;action=modify",
                    'image'=>'small-admin',
                    'section'=>'Tasks',
                    'onclick'=>"document.getElementById('modify').style.display='';return false");
  }
  if ($cameralife->Security->authorize('admin_albums') && (preg_match("#album.php\?id=([0-9]+)#",$_SERVER['HTTP_REFERER'],$regs) || preg_match("#albums/([0-9]+)#",$_SERVER['HTTP_REFERER'],$regs)))
  {
    $album = new Album($regs[1]);
    $icon = $album->GetIcon('small');
#TODO a separate controller?
    if (strpos($icon['href'], '&#63;') !== FALSE)
      $icon['href'] .= '&poster_id='.$photo->Get('id');
    else 
      $icon['href'] .= '?poster_id='.$photo->Get('id');
    $icon['name'] = 'Use this for <b>'.$album->Get('name').'</b>';
    $icon['section'] = 'Tasks';
    $menu[] = $icon;
  }
  if ($photo->Get('status') == 0)
  {
    $icon = $photo->GetIcon();
    if (strpos($icon['href'], '&#63;') !== FALSE)
      $icon['href'] .= '&amp;referer='.urlencode($_SERVER['HTTP_REFERER'])."&amp;action=print";
    else
      $icon['href'] .= '&#63;referer='.urlencode($_SERVER['HTTP_REFERER'])."&amp;action=print";


    $menu[] = array('name'=>'Order prints',
                    'href'=>$icon['href'],
                    'image'=>'small-admin',
                    'section'=>'Tasks');
  }


  $context = $photo->GetRelated();
  foreach ($context as $icon)
  {
    $icon['section'] = 'Related Photos';
    $menu[] = $icon;
  }
  $photoPrev = $photo->GetPrevious();
  $photoNext = $photo->GetNext();


?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
  <title><?= htmlentities($photo->Get('description')) ?></title>
  <?php if($cameralife->Theme->cssURL()) {
    echo '  <link rel="stylesheet" href="'.$cameralife->Theme->cssURL()."\">\n";
  } ?>
  <meta http-equiv="Content-Type" content="text/html; charset= ISO-8859-1">
  <link rel="stylesheet" type="text/css" href="delicious.css">
<?php
  if($photoPrev)
  {
    $icon = $photoPrev->GetIcon();
    if (strpos($icon['href'], '&#63;') !== FALSE)
      $hrefinqueryTMP = $icon['href'] . '&amp;';
    else
      $hrefinqueryTMP = $icon['href'] . '&#63;';
    $href = $hrefinqueryTMP.'referer='.urlencode($_SERVER['HTTP_REFERER']);
    echo "  <link rel=\"prev\" href=\"$href\">\n";
  }
  if($photoNext)
  {
    $icon = $photoNext->GetIcon();
    if (strpos($icon['href'], '&#63;') !== FALSE)
      $hrefinqueryTMP = $icon['href'] . '&amp;';
    else
      $hrefinqueryTMP = $icon['href'] . '&#63;';
    $href = $hrefinqueryTMP.'referer='.urlencode($_SERVER['HTTP_REFERER']);
    echo "  <link rel=\"next prefetch\" href=\"$href\">\n";
  }
?>
  <script type="text/javascript" src="<?= $cameralife->base_url . '/modules/theme/' . $cameralife->GetPref('theme') ?>/keywords.php"></script>
  <script type="text/javascript" src="<?= $cameralife->base_url . '/modules/theme/' . $cameralife->GetPref('theme') ?>/delicious.js"></script>
  <script type="text/javascript" src="<?= $cameralife->base_url . '/modules/theme/' . $cameralife->GetPref('theme') ?>/photo.js"></script>
  <script type="text/javascript" src="<?= $cameralife->base_url . '/modules/theme/' . $cameralife->GetPref('theme') ?>/ajax.js"></script>
  <script type="text/javascript">
<?php
  $suggestions = $photo->Get('path');

  foreach (split('/', $suggestions) as $keyword)
    $keys['"'.eregi_replace('[^a-z0-9]','',$keyword).'"']=1;
  echo 'var copytags=['.implode(',',array_keys($keys))."]\n";
  echo "var rating=".($rating+0)."\n";
  echo 'var base_url="'.$cameralife->base_url.'"';
?>
  </script>
</head>
<body onLoad="init()">
<?php
  $cameralife->Theme->TitleBar(htmlentities($photo->Get('description')),
                               'photo',
                               "(".$photo->Get('created').")",
                               $menu);

  if ($photo->Get('status') > 0)
  {
?>
    <div class="administrative" align=center>
      <p>This photo is not public</p>
    </div>
<?php
  }
  if ($cameralife->Security->authorize('photo_rename'))
  {
?>
    <form action="<?= $cameralife->base_url ?>/photo_controller.php" method="POST" name="form" id="renameform">
    <input type="hidden" name="id" value="<?= $photo->Get('id') ?>">
    <input type="hidden" name="target" value="<?= $cameralife->base_url.'/photo.php&#63;id='.$photo->Get('id').'&referer='.urlencode($_SERVER['HTTP_REFERER']) ?>">
    <input type="hidden" name="action" value="rename">

    <div style="<?=($_GET['action'] == 'rename')?'':'display:none'?>" class="administrative" align=center id="rename">
      <table>
        <tr><td colspan=3><p>This <b>Title</b> will be shown on this page and on thumbnails of this photo elsewhere. <b>Keywords</b> will never be displayed. Searches and Albums both use the <b>Title</b> and <b>Keywords</b> to find photos.</p>
        <tr><td>Title:<td colspan=2><input id="formtitle" style="width: 100%;" type="text" name="param1" value="<?= htmlentities($photo->Get('description')) ?>">
      <?php
        $origname = ucwords($photo->Get('filename'));
        $origname = preg_replace('/.[^.]+$/', '', $origname);

        if (!eregi('^dscn', $origname) && !eregi('^im', $origname)) // useless filename
          echo '<tr><td><td><input type="button" onclick="javascript:set(\''.addslashes($origname).'\')" value="Set name to '.$origname.'">';
      ?>
        <tr><td>Keywords:
          <td style="width:100%"><input style="width: 100%;" id="tags" name="param2" value="<?= htmlentities($photo->Get('keywords')) ?>" autocomplete="off" type="text">
          <td>space&nbsp;separated
        <tr style="visibility: hidden; background:pink" id="suggestions">
          <td class="rs">completions
          <td><div id="suggest"></div>
        <tr>
          <td><i>suggested</i>
          <td colspan=2 style="border-bottom: 1px solid #2F4C5D;">
            <span id="copy">
            </span>
        <tr valign=top>
          <td>
              <i><a id="alphasort" class="noclicky" href="javascript:sort('alpha')">all</a
              >&nbsp;|&nbsp;<a id="freqsort" class="clicky" href="javascript:sort('freq')">common</a></i>
          <td colspan=2>
            <div id="alpha">
            </div>
        <tr><td>&nbsp;

        <tr><td><td>
          <input type="submit" value="Save Changes" onclick="makeRequest('<?= $cameralife->base_url ?>/photo_controller.php','renameform',renameDone);return false">
          <a href="<?= $icon['href'] ?>"
             onclick="return toggleshowrename()">(cancel)</a> 
      </table>
    </div>
    </form>
<?php
  }
  if ($cameralife->Security->authorize('photo_delete'))
  {
    if ($photoNext)
    {
      $tmp = $photoNext->GetIcon();
      $target = $tmp['href'];
    }
    elseif ($photoPrev)
    {
      $tmp = $photoPrev->GetIcon();
      $target = $tmp['href'];
    }
    elseif ($_SERVER['HTTP_REFERER'])
      $target = $_SERVER['HTTP_REFERER']; #wont work
    else
      $target = $cameralife->base_url.'/index.php';
?>
    <form action="<?= $cameralife->base_url ?>/photo_controller.php" method="POST">
    <input type="hidden" name="id" value="<?= $photo->Get('id') ?>" />
    <input type="hidden" name="target" value="<?= $hrefinquery . 'referer='.urlencode($_SERVER['HTTP_REFERER']) ?>" />
    <input type="hidden" name="action" value="flag" />

    <div style="<?=($_GET['action'] == 'delete')?'':'display:none'?>"class="administrative" align=center id="delete">
      <p>Thank you for taking the time to report bad content. Please choose the obvious problem with this photo.</p>
      <table>
        <tr><td><input type="radio" name="param1" id="r1" value="indecent">
          <td><label for="r1">Indecent or Sensitive</label>
          <td>Obsenity, nudity
        <tr><Td><input type="radio" name="param1" id="r2" value="photography">
          <td><label for="r2">Bad photography</label>
          <td>Blurry, poor lighting, terrible cropping
        <tr><Td><input type="radio" name="param1" id="r3" value="subject">
          <td><label for="r3">Poor subject</label>
          <td>The subject is uninteresting, ie: dirt
        <tr><Td><input type="radio" name="param1" id="r4" value="bracketing">
          <td><label for="r4">Bracketing</label>
          <td>A very similar photo supercedes this one
      </table>
          <p><input type="submit" value="Flag Photo">
          <a href="<?= $hrefinquery . 'referer='.urlencode($_SERVER['HTTP_REFERER']) ?>"
           onclick="document.getElementById('delete').style.display='none';return false">(cancel)</a> </p>
    </div>
  </form>
<?php
  }
  if ($cameralife->Security->authorize('photo_modify'))
  {
?>
    <div style="<?=($_GET['action'] == 'modify')?'':'display:none'?>"class="administrative" align=center id="modify">
      <form action="<?= $cameralife->base_url ?>/photo_controller.php" method="POST">
      Rotate: <input type="hidden" name="id" value="<?= $photo->Get('id') ?>" />
      <input type="hidden" name="target" value="<?= $hrefinquery . 'referer='.urlencode($_SERVER['HTTP_REFERER']) ?>" />
      <input type="hidden" name="action" value="rotate" />
        <input type="submit" name="param1" value="Rotate Counter-Clockwise">
        <input type="submit" name="param1" value="Rotate Clockwise">
      </form>

      <form action="<?= $cameralife->base_url ?>/photo_controller.php" method="POST">
      Revert: <input type="hidden" name="id" value="<?= $photo->Get('id') ?>" />
      <input type="hidden" name="target" value="<?= $hrefinquery . 'referer='.urlencode($_SERVER['HTTP_REFERER']) ?>" />
      <input type="hidden" name="action" value="revert" />
        <input type="submit" name="param1" value="Revert">
      </form>

      <form action="<?= $cameralife->base_url ?>/photo_controller.php" method="POST">
      Regenerate Thumbnail: <input type="hidden" name="id" value="<?= $photo->Get('id') ?>" />
      <input type="hidden" name="target" value="<?= $hrefinquery . 'referer='.urlencode($_SERVER['HTTP_REFERER']) ?>" />
      <input type="hidden" name="action" value="rethumb" />
        <input type="submit" name="param1" value="Regenerate Thumbnail">
      </form>

      <a href="<?= $hrefinquery . 'referer='.urlencode($_SERVER['HTTP_REFERER']) ?> "
         onclick="document.getElementById('modify').style.display='none';return false">(cancel)</a> 
    </div>
<?php
  }
  elseif ($ask_user_to_login == true)
  {
?>
    <div class="administrative" align=center>
      Thank you for your contributions to <?= $cameralife->GetPref('sitename') ?>, please consider
      <?php $cameralife->Theme->Image('small-login',array('align'=>'middle')) ?>
      <b><a href="login.php">registering or logging in</a></b>.
    </div>
<?php
  }
  if ($photo->Get('status') == 0 && $_GET['action'] == 'print')
  {
?>
    <div class="administrative" align=center>
    <form action="http://www.digibug.com/dapi/order.php" method="post">
    <p>You can order prints of this photo from the Camera Life store.</p>
      <input type="hidden" name="digibug_api_version" value="100">
      <input type="hidden" name="company_id" value="1941">
      <input type="hidden" name="event_id" value="5755">
      <input type="hidden" name="cmd" value="addimg">
      <input type="hidden" name="return_url" value="<?= $cameralife->base_url.'/photo.php&#63;id='.$photo->Get('id')."&amp;referer=".urlencode($_SERVER['HTTP_REFERER']) ?>">
      <input type="hidden" name="num_images" value="2">
      <input type="hidden" name="image_1" value="<?= $cameralife->base_url.'/'.$photo->GetMedia('photo') ?>">
      <input type="hidden" name="image_height_1" value="<?= $photo->Get('height') ?>">
      <input type="hidden" name="image_width_1" value="<?= $photo->Get('width') ?>">
      <input type="hidden" name="thumb_1" value="<?= $cameralife->base_url.'/'.$photo->GetMedia() ?>">
      <input type="hidden" name="thumb_height_1" value="<?= $photo->Get('tn_height') ?>">
      <input type="hidden" name="thumb_width_1" value="<?= $photo->Get('tn_width') ?>">
      <input type="hidden" name="title_1" value="<?= $photo->Get('description') ?>">
      <input type="submit" value="Order">
    </form>
    </div>
<?php
  }

  
  if (isset($cameralife->receipt))
  {
    echo '<div id="receipt" class="receipt">';
    echo '<form action="'.$cameralife->base_url.'/undo_controller.php" method="post">';
    echo '<input id="receiptid" type="hidden" name="id" value="'.$cameralife->receipt->Get('id').'" />';
    echo '<input type="hidden" name="action" value="undo" />';
    echo '<input type="hidden" name="target" value="'.$hrefinquery . 'referer='.urlencode($_SERVER['HTTP_REFERER']).'" />';
    echo '<span id="receipttext">'.$cameralife->receipt->GetDescription().'</span> <button type="submit">Undo</button></form></div>';
  }
  else
  { 
    echo '<div id="receipt" class="receipt" style="visibility: hidden"></div>';
  }

  echo "<div style=\"text-align: center\">\n";
  if($photoPrev)
  {
    $icon = $photoPrev->GetIcon();

    if (strpos($icon['href'], '&#63;') !== FALSE)
      $hrefinqueryTMP = $icon['href'] . '&amp;';
    else
      $hrefinqueryTMP = $icon['href'] . '&#63;';
    $href = $hrefinqueryTMP.'referer='.urlencode($_SERVER['HTTP_REFERER']);

    $src = $icon['image'];
    $alt = htmlentities($icon['name']);
    echo "<a href='$href'><img id=\"prevphoto\" style=\"margin: 0 10px\" src='$src' alt=\"$alt\"></a>\n\n";
  }
  $alt = htmlentities($photo->Get('description'));
  echo "<img id=\"curphoto\" style=\"margin: 0 auto\" src=\"".$photo->GetMedia('scaled')."\" alt=\"$alt\">\n\n";
  if($photoNext)
  {
    $icon = $photoNext->GetIcon();

    if (strpos($icon['href'], '&#63;') !== FALSE)
      $hrefinqueryTMP = $icon['href'] . '&amp;';
    else
      $hrefinqueryTMP = $icon['href'] . '&#63;';
    $href = $hrefinqueryTMP.'referer='.urlencode($_SERVER['HTTP_REFERER']);

    $src = $icon['image'];
    $alt = htmlentities($icon['name']);
    echo "<a href='$href'><img id=\"nextphoto\" style=\"margin: 0 10px\" src='$src' alt=\"$alt\"></a>";
  }
  echo "</div>\n";
?>

<table width="100%">
  <tr valign=top>
    <td width="49%">
      <?php $cameralife->Theme->Section('Information'); ?>

      <div class="context">
        <a rel="nofollow" href="<?= $photo->GetMedia('photo') ?>">
          <?php $cameralife->Theme->Image('small-photo',array('align'=>'middle')) ?>
          View full-sized photo (<?= $photo->Get('width')." by ".$photo->Get('height') ?> pixels)
        </a>
      </div>
      <?php
        $sizes = preg_split('/[, ]+/',$cameralife->GetPref('optionsizes'));
        sort($sizes);
        if (count($sizes)) foreach ($sizes as $size)
        {
          echo " <div class=\"context\">"; 
          echo " <a rel=\"nofollow\" href=\"".$photo->GetMedia($size)."\">";
          $cameralife->Theme->Image('small-photo',array('align'=>'middle'));
          echo " View at ${size}px</a>\n";
          echo "</div>";
        }
      ?>
      <?php
        if ($photo->Get('username'))
        {
          echo '<div class="context">';
          $cameralife->Theme->Image('small-photo',array('align'=>'middle'));
          echo 'This photo was added by '.$photo->Get('username');
          echo '</div>';
        }

        if ($exif = $photo->GetEXIF())
        {
          echo '<div class="context">';
          foreach ($exif as $key=>$val)
          {
            $cameralife->Theme->Image('small-photo',array('align'=>'middle'));
            echo "$key: $val<br />\n";
          }
          echo '</div>';
        }

      ?>
    <td width="2%">
    <td width="49%">
    <form action="<?= $cameralife->base_url?>/photo_controller.php" method=POST name="form">
    <input type="hidden" name="id" value="<?= $photo->Get('id') ?>">
    <input type="hidden" name="target" value="<?= $hrefinquery . 'referer='.urlencode($_SERVER['HTTP_REFERER']) ?>">
    <input type="hidden" name="action" value="rate">
      <?php 
        $cameralife->Theme->Section('Feedback'); 

  echo "My rating:\n";
  $unlit = $cameralife->Theme->ImageUrl('unlit');
  $lit = $cameralife->Theme->ImageUrl('lit');

  echo "<span id=\"rating\">";
  for ($i=1;$i<=5;$i++)
  {
    $hrefjs = ($i!=1) ? '' : "onclick=\"document.getElementById('delete').style.display='';return false\"";
    $style = 'style="border:0; background:none; padding:0; margin:0"';
    $imgsrc = ($i <= $rating) ? $lit : $unlit;
    $img = "<img id=\"rate$i\" src=\"$imgsrc\" alt=\"star\">";
 
    echo "<button name=\"param1\" value=\"$i\" type=\"submit\" onmouseover=\"overstar($i)\" onmouseout=\"nostar()\" $style>$img</button>";
  }
  if ($rating == 5)
    echo "(saved in my favorite photos)";

  echo "</span><br>";
  $avg = $cameralife->Database->SelectOne('ratings', 'AVG(rating)', 'id='.$photo->Get('id'));
  echo "Average rating: $avg\n";
  echo "<p>";
$result = $cameralife->Database->Select('comments', '*', 'photo_id='.$photo->Get('id'));
while ($comment = $result->FetchAssoc())
echo "<strong>".$comment['username']."</strong> <em>" . $comment['date']."</em><br>" .htmlentities($comment['comment']) . "<hr>";
  echo "</p>";
      ?>
</form>

<form action="<?= $cameralife->base_url ?>/photo_controller.php" method=POST name="form">
<input type="hidden" name="id" value="<?= $photo->Get('id') ?>">
<input type="hidden" name="target" value="<?= $hrefinquery . 'referer='.urlencode($_SERVER['HTTP_REFERER']) ?>">
<input type="hidden" name="action" value="comment">
<input name="param1">
<input type="submit" value="Post comment">
</form>
    </table>

<?php
  # Cache the next image the user is likely to look at
  if ($photoNext)
  {
    echo '<img style="display:none" src="'.$photoNext->GetMedia('scaled').'" alt="hidden photo">';
//    echo '<input type="button" value="Push me" onClick="fadein('.$photo->Get('tn_width').','.$photoNext->Get('tn_width').')">';
  }
/*  if ($photoPrev)
    echo '<img style="display:none" src="'.$photoPrev->GetMedia('scaled').'">';*/
?>

</body>
</html>
