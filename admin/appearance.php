<?php
  # Sets the options for your site...

  $features=array('database','security','theme');
  require "../main.inc";
  $cameralife->base_url = dirname($cameralife->base_url);

  $cameralife->Security->authorize('admin_customize', 1); // Require

  $_GET['page'] or $_GET['page'] = 'setup';

  foreach ($_POST as $key => $val)
  {
    if ($key == 'UseTheme')
    {
      $cameralife->preferences['core']['theme'] = $val;
      header('Location: customize.php?page=themes');
    }
    else
      $cameralife->preferences['core'][$key] = rtrim($val,'/');
  }
  $cameralife->SavePreferences();

  function check_dir($dir)
  {
    global $cameralife;

    if ($dir[0] != '/')
      $dir = $cameralife->base_dir."/$dir/";
    if (!is_dir($dir) )
      echo "<p class=\"alert\">WARNING: $dir is not a directory!</p>";
	elseif (!is_writable($dir))
      echo "<p class=\"alert\">WARNING: $dir is not writable!</p>";
  }

  $all_themes = array();
?>

<html>
<head>
  <title><?= $cameralife->preferences['core']['sitename'] ?></title>
  <link rel="stylesheet" href="admin.css">
  <meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1">
</head>
<body>

<div id="header">
<h1>Site Administration &ndash; Theme</h1>
<?php
  $home = $cameralife->GetSmallIcon();
  echo '<a href="'.$home['href']."\"><img src=\"".$cameralife->IconURL('small-main')."\">".$home['name']."</a>\n";
?> |
<a href="index.php"><img src="<?= $cameralife->IconURL('small-admin')?>">Site Administration</a> | <a href="http://fdcl.sourceforge.net/index.php&#63;content=themes">Get more themes</a>
</div>

<form method="post" action="controller_prefs.php">
<input type="hidden" name="target" value="<?= $_SERVER['PHP_SELF'].'&#63;page='.$_GET['page'] ?>" />
<input type="hidden" name="module1" value="core" />
<input type="hidden" name="param1" value="theme" />
<input type="hidden" name="module2" value="core" />
<input type="hidden" name="param2" value="iconset" />
<table>
  <tr>
    <td>Choose a theme engine
    <td>
      <select name="value1">
      <?php 
        $themes = glob($cameralife->base_dir."/modules/theme/*");
        foreach($themes as $theme)
        {
          if ($file[0] == '.')
            continue;
          if (!is_dir($theme))
            continue;
          if (!is_file($theme."/theme-info.php"))
            continue;
    
          include($theme."/theme-info.php");
    
          if ($cameralife->preferences['core']['theme'] == basename($theme))
            echo "<option selected value=\"".basename($theme)."\">\n";
          else
            echo "<option value=\"".basename($theme)."\">\n";
    
          echo "<b>$theme_name</b> - <i>version $theme_version by $theme_author</i>";
          echo "</option>\n";
          flush();
        }

      ?>
      </select>
    <td><input type="submit" value="Choose">
  
  <tr>
    <td>Choose an iconset
    <td>
      <select name="value2">
      <?php
        $themes = glob($cameralife->base_dir."/modules/iconset/*");
        foreach($themes as $theme)
        {
          if ($file[0] == '.')
            continue;
          if (!is_dir($theme))
            continue;
          if (!is_file($theme."/iconset-info.php"))
            continue;

          include($theme."/iconset-info.php");

          if ($cameralife->preferences['core']['iconset'] == basename($theme))
            echo "<option selected value=\"".basename($theme)."\">\n";
          else
            echo "<option value=\"".basename($theme)."\">\n";

          echo "<b>$iconset_name</b> - <i>version $iconset_version by $iconset_author</i>";
          echo "</option>\n";
          flush();
        }
      ?>
      </select>
    <td><input type="submit" value="Choose">
     
</table>
</form>

<form method="post" action="controller_prefs.php">
<input type="hidden" name="target" value="<?= $_SERVER['PHP_SELF'].'&#63;page='.$_GET['page'] ?>" />
<h2>Site Parameters</h2>
<table>
  <tr>
    <td>Site name
    <td>
      <input type="hidden" name="module1" value="core" />
      <input type="hidden" name="param1" value="sitename" />
      <input type=text name="value1" size=30 value="<?= $cameralife->preferences['core']['sitename'] ?>">
  <tr>
    <td>Site abbreviation (used to refer to the main page)
    <td>
      <input type="hidden" name="module2" value="core" />
      <input type="hidden" name="param2" value="siteabbr" />
      <input type=text name="value2" size=30 value="<?= $cameralife->preferences['core']['siteabbr'] ?>">
  <tr>
    <td>Owner E-mail address (shown if something goes wrong)
    <td>
      <input type="hidden" name="module3" value="core" />
      <input type="hidden" name="param3" value="owner_email" />
      <input type=text name="value3" size=30 value="<?= $cameralife->preferences['core']['owner_email'] ?>">
  <tr>
    <td>Use pretty URL's (requires mod rewrite)
    <td>
      <input type="hidden" name="module4" value="core" />
      <input type="hidden" name="param4" value="rewrite" />
      <select name="value4">
        <option <?= $cameralife->preferences['core']['rewrite'] == 'no' ? 'selected="selected"':'' ?>>no</option>
        <option <?= $cameralife->preferences['core']['rewrite'] == 'yes' ? 'selected="selected"':'' ?>>yes</option>
      </select>
  <tr>
    <td><td><input type="submit" value="Save changes" />
</table>
</form>


<form method="post" action="controller_prefs.php">
<input type="hidden" name="target" value="<?= $_SERVER['PHP_SELF'].'&#63;page='.$_GET['page'] ?>" />
<h2>Settings for <?= $cameralife->preferences['core']['theme'] ?></h2>
<table>
<?php
  $prefnum=0;
  foreach ($cameralife->Theme->preferences as $pref)
  {
    $prefnum++;
    echo "  <tr><td>".$pref['desc']."\n";
    echo "    <td>\n";
    echo "      <input type=\"hidden\" name=\"module$prefnum\" value=\"theme\" />\n";
    echo "      <input type=\"hidden\" name=\"param$prefnum\" value=\"".$pref['name']."\" />\n";
 
    if ($cameralife->preferences['theme'][$pref['name']])
      $value = $cameralife->preferences['theme'][$pref['name']];
    else
      $value = $pref['default'];

    if ($pref['type'] == 'number' || $pref['type'] == 'string')
    {
      echo "      <input type=\"text\" name=\"value$prefnum\" value=\"$value\" />\n";
    }
    elseif (is_array($pref['type'])) // enumeration
    {
      echo "      <select name=\"value$prefnum\" />\n";
      foreach($pref['type'] as $index=>$desc)
      {
        if ($index == $value)
          echo "        <option selected value=\"$index\">$desc</option>\n";
        else
          echo "        <option value=\"$index\">$desc</option>\n";
      }
      echo "      </select />\n";
    }
  }
?>
  <tr><td><td><input type="submit" value="Save changes" />
</table>
</form>


<form method="post" action="controller_prefs.php">
<h1>RED RED MOE ME</h1>


  <table align="center" cellspacing="2" border=1 width="100%">
    <tr>
      <th colspan=2>
        Site directories - <i>relative to the main page</i>
    <tr>
      <td>
        Main photo directory
      <td width=100>
        <input type=text name="photo_dir" size=30
                value="<?= $cameralife->preferences['core']['photo_dir'] ?>">
    <tr>
      <td>
        Camera Life data directory
      <td>
        <input type=text name="cache_dir" size=30
                value="<?= $cameralife->preferences['core']['cache_dir'] ?>">
    <tr>
      <td>
        Deleted photos (...where they go when you "erase" them)
      <td>
        <input type=text name="deleted_dir" size=30
                value="<?= $cameralife->preferences['core']['deleted_dir'] ?>">
  </table>

  <p>
    <input type=submit value="Save and Validate changes">
    <a href="customize.php">(Revert to last saved)</a>
  </p>

</body>
</html>

