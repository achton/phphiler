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
//echo "<pre>";
//echo "strpos: $strpos <br>";
//echo "</pre>";
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
//echo "<pre>";
//print_r($fileinfo);
//echo "</pre>";
  return $fileinfo;
}

// særlig funktion, der anvendes af PHPHiler på alle biblioteker i navigations-menuen
// (se header.inc) - den kopierer den globale index fil ned i biblioteket, hvis den ikke
// findes der i forvejen
function IndexCopy ($dir) {
  // giv adgang til et par variable fra globals.inc
  global $MAIN_INDEX_FILE;
  global $MAIN_INDEX_PATH;
  // opbyg den fulde sti til den fil vi ønsker at tjekke for
  $target_path = getcwd() . '/' . $dir . '/' . $MAIN_INDEX_FILE;
  // hvis filen ikke eksisterer, kopierer vi den ned i biblioteket
  if (!file_exists($target_path)) {
    copy ($MAIN_INDEX_PATH . $MAIN_INDEX_FILE, $target_path);
  }
  //echo $target_path . "<br>";

  // lad PHPhiler defaulte til standard titel funktionen (PHPH_filename), se PHPhiler docs
  return FALSE;
}

?>