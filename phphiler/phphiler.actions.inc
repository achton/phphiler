<?php

// This action function translates a Windows .lnk-file (Internet Shortcut)
// into a HTML href that is clickable like any other link.
function WinLnkTranslator ($file) {
  // read the file into an array
  $file_ar = file($file);

  // find the proper section in the file and get the URI
  foreach ($file_ar as $line) {
    if ($pos = strpos($line, '[InternetShortcut]') !== FALSE) {
      $string = next($file_ar);
      if ($pos = strpos($string, 'URL=') !== FALSE) {
        // we got the URI keyword, let's get the link
        $uri = substr($string, $pos+3);
        break;
      }
    }
    next($file_ar);
  }
  // make the title the name of the link (uses a function from phphiler.functions.inc)
  $title = stripSuffix($file);
  // enter stuff into the array and return it to PHPhiler
  $fileinfo = array('uri' => $uri, 'title' => $title, 'description' => $title);
  return $fileinfo;
}

// This is another action function that gets the title and description
// of a file from an external description file. It must take the filename
// itself as argument and returns an array containing the title and
// description.
function PHPH_NFOfile ($file) {
  // Define the suffix of description files. This should differ from all
  // suffixes in the setSuffixSettings array.
  $suffix = 'nfo';

  // strip the suffix
  $file = stripSuffix($file);

  // define the comment file to read tags from
  $commentfile = $file . '.' . $suffix;
  if (!file_exists($commentfile)) {
    return FALSE;
  }

  // Set the tags that define the title and description in an external file
  // (e.g. 'title', which expands to '<title>' and '</title>').
  $keyarray = array('title', 'description');

  // try to open the commentfile, but don't warn or exit on error (it may not be there)
  $fp = fopen($commentfile, 'r');

  // if the commentfile exists, go on
  if ($fp) {
    // read the file into the string in 4k blocks
    while (!feof($fp)) {
      $string .= fread($fp, 4096);
    }
    // close the file
    fclose($fp) or exit('ERROR: Unable to close file '.$commentfile);

    foreach ($keyarray as $key ) {
      $tag = '<'.$key.'>';
      $endtag = '</'.$key.'>';
      // remove the begin tag from the string
      $strpos = strpos($string, $tag);
      // if there was no begin tag this time, go back and try the next tag
      if ($strpos === FALSE) continue;
      $string = substr($string, $strpos+strlen($tag));
      // save everything until the end tag
      $strpos = strpos($string, $endtag);
      if ($strpos === FALSE) return FALSE;
      // enter the stuff between the tags into the array
      $fileinfo[$key] = substr($string, 0, $strpos);
    }
  }
  return $fileinfo;
}

// Special action that can check for the presence of a special file in all
// subdirectories of a file list and copy the file there if it is not present.
function IndexCopy ($dir) {
  // give access to some custom global variables
  global $MAIN_INDEX_FILE;
  global $MAIN_INDEX_PATH;
  // build the full target path for copying the file
  $target_path = getcwd() . '/' . $dir . '/' . $MAIN_INDEX_FILE;
  // if the file does not exist, we'll copy it into the target directory
  if (!file_exists($target_path)) {
    copy ($MAIN_INDEX_PATH . $MAIN_INDEX_FILE, $target_path);
  }

  // let PHPhiler default to the standard file retrieval function by returning
  // false, as if this function failed (although it probably did what it was
  // supposed to)
  return FALSE;
}

?>