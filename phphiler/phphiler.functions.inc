<?php
/*
PHPhiler - a dynamic engine for file and directory indexing with PHP

Copyright (c) 2003-2004 Achton N. Netherclift

This file is part of PHPhiler.

PHPhiler is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

Contact the author via e-mail at  : geddeth@users.sourceforge.net
Project homepage is found at      : http://phphiler.sourceforge.net

*/

// This is the list of general utility functions for PHPhiler. Note that these
// are not in a class by themselves.

  // The default action function that gets the title of a file
  // from the filename. It must take the filename itself as argument and
  // returns an array containing the title.
  function PHPH_filename ($file) {
    $string = stripSuffix($file);
    $fileinfo = array('title' => $string);
    return $fileinfo;
  }

  // Function that checks whether a certain bit is set.
  // Takes as parameters a string of bits, e.g. "01011" and which bit number
  // to check  (bit 0 is the rightmost bit)
  function isBitSet($bits, $bitno) {
    // get the length of the string
    $length = strlen($bits);
    // check if the bit number is valid
    if ($bitno > $length-1 || $bitno < 0) exit("ERROR: isBitSet(): Bit position does not exist!");
    // return true if it's set and false otherwise
    return (substr($bits, $length-($bitno+1), 1) === "1");
  }


  // Function that simply checks an associative array of keys and bitstrings,
  // returning true if the bit number given is set in one or more of the strings.
  function isBitSetInArray($ar, $bitno) {
      foreach($ar as $key => $bits) {
      if (isBitSet($bits, $bitno)) return true;
    }
    return false;
  }

  // Function that reads the modify date of a file "l, F jS, Y"
  function readFileDate($filename) {
    $filedate = filemtime($filename);
    return $filedate;
  }


  // Function to return the filename minus suffix
  function stripSuffix($file) {
    $pos = strrpos($file, ".");
    if ($pos !== FALSE) {
      return substr($file, 0, $pos);
    } else return $file;
  }


  function roundSize ($sSize) {

    $string = NULL;
    // the following contributed by brodie-php at tmts dot ca (php.net):
    // setup some common file size measurements
    $kb = 1024;
    $mb = 1048576;
    $gb = 1073741824;
    $tb = 1099511627776;

    if($sSize < $kb) {
      $string = $sSize . ' B';
    }
    elseif($sSize < $mb) {
      $string = round($sSize/$kb, 2).' KB';
    }
    elseif($sSize < $gb) {
      $string = round($sSize/$mb, 2).' MB';
    }
    elseif($sSize < $tb) {
      $string = round($sSize/$gb, 2).' GB';
    }
    else {
      $string = round($sSize/$tb, 2).' TB';
    }
    return $string;
  }


  function translate_uri ($uri) {
    $parts = explode('/', $uri);
    for ($i = 0; $i < count($parts); $i++) {
      $parts[$i] = rawurlencode($parts[$i]);
    }
    return implode('/', $parts);
  }

  function translate_string ($string, $path_func, $path_func_ar) {
    if (empty($string))
      exit ("<b style=\"color:red;\">ERROR:</b> Could not translate string because it's empty!<br>");
    // if the function or argument array is not present, we don't want
    // to translate the string
    if (empty($path_func) || empty($path_func_ar))
      return $string;
    // put the string to be translated first in the array
    array_unshift($path_func_ar, $string);
    // call the user function and return the output
    return call_user_func_array($path_func, $path_func_ar);
  }
?>