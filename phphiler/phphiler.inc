<?php
/*
PHPhiler - a dynamic engine for file and directory indexing with PHP

Copyright (c) 2003-2004 Achton N. Netherclift

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

require_once('phphiler.functions.inc');
require_once('phphiler.html.inc');

/**
 * PHPhiler is a directory indexing system
 *
 * The PHPhiler class is the main object constructed for file and directory
 * indexing. The constructor of a PHPhiler object sets the main options and
 * waits for a function call to its index() function. This file should be
 * included in any document that displays the index (e.g. makes a call to
 * index()).
 *
 * @package     PHPhiler
 * @version     $Id$
 * @author      Achton N. Netherclift <achton@netherclift.net>
 * @access      public
 */
class PHPhiler {

  // private variables
  var $__phphiler_ver       = '0.3.0'; // version number
  var $__phphiler_link      = 'http://phphiler.sf.net'; // link address

  var $__statusbar_comment  = NULL;     // what to display in the status bar (string/keyword)

  var $__sortcolumn         = NULL;     // the column to sort by (string/keyword)
  var $__sortorder          = NULL;     // the order to sort by (asc/desc) (string/keyword)

  var $__include_sub_dirs   = NULL;     // whether to include directories (bool)
  var $__include_files      = NULL;     // whether to include files (bool)
  var $__include_table_header = NULL;   // whether to include the table header columns (bool)
  var $__include_table_footer = NULL;   // whether to include the table footer (bool)

  var $__header_path        = NULL;     // path to header file (string)
  var $__footer_path        = NULL;     // path to footer file (string)

  var $__total_entries      = NULL;     // number of files in this listing ($int)

  var $__debug_mode         = false;     // debug mode on/off (bool)

  var $files                = array();  // actual filenames and doctitles ('file' => 'title')
  var $dirs                 = array();  // dirs and corresponding titles ('dir' => 'title')
  var $filepaths            = array();  // relative paths to files ('file/dir' => 'path')
  var $filedescr            = array();  // descriptions ('file/dir' => 'descr')
  var $filedates            = array();  // date ('file/dir' => 'date')
  var $filesizes            = array();  // size ('file/dir' => 'size)
  var $fileactions          = array();  // actions ('file' => 'action_bits')
  var $fileuris             = array();  // uris ('file' => 'uri')

  var $file_self            = NULL;     // filename of the working document
  var $cwd                  = NULL;     // current working directory
  var $cwrd                 = NULL;     // current relative working directory

  var $configfilename       = 'phphiler.config.inc';    // filename of the configuration file
  var $templatefilename     = 'phphiler.template.php';  // filename of template

  var $starttime            = NULL;     // start time for the execution time measurement

  // This is the class constructor. It sets the various static variables and constants.
  function PHPhiler($startdir = NULL) {

    // first initialize the time counter
    $mtime = microtime();
    $mtime = explode(' ',$mtime);
    $mtime = $mtime[1] + $mtime[0];
    $this->starttime = $mtime;

    // clear PHP's file status cache
    clearstatcache();

    // constants
    define ('PHPH_TITLE', 'title');
    define ('PHPH_SIZE', 'size');
    define ('PHPH_DATE', 'date');
    define ('PHPH_DESCR', 'descr');

    define ('PHPH_ASC', 'asc');
    define ('PHPH_DESC', 'desc');

    define ('PHPH_DIR', 'dir');

    unset($GLOBALS['PHPH_CSS_SUFFIX']);

    // include the configuration file
    if (isset($GLOBALS['PHPH_CONFIG_FILE'])) {
        $this->configfilename = $GLOBALS['PHPH_CONFIG_FILE'];
    } else {
        exit("<b style=\"color:red;\">ERROR:</b> \$GLOBALS['PHPH_CONFIG_FILE'] is not set.<br>");
    }
    require($this->configfilename);

    // set the global constants for version no. and project link
    define('PHPHILER_LINK', $this->__phphiler_link);
    define('PHPHILER_VER', $this->__phphiler_ver);

    $doc_root = $this->getDocRoot();
    if (!$doc_root) {
      $doc_root = $_SERVER['DOCUMENT_ROOT'] . '/';
    }

    if (!file_exists($this->getDocRoot() . $this->getPHPhilerPath() . 'phphiler.inc')) {
        exit("<b style=\"color:red;\">ERROR:</b> Cannot find PHPhiler source. Please check config of setPHPhilerPath().<br>");
    }

    // Include actions if they exist
    if (file_exists($this->getDocRoot() . $this->getPHPhilerPath() . 'phphiler.actions.inc')) {
        require_once($this->getDocRoot() . $this->getPHPhilerPath() . 'phphiler.actions.inc');
    }
    if (file_exists($this->getDocRoot() . $this->getPHPhilerPath() . 'phphiler.custom.actions.inc')) {
        require_once($this->getDocRoot() . $this->getPHPhilerPath() . 'phphiler.custom.actions.inc');
    }

    // pop the trailing slash off the end
    $doc_root_notrail = substr($doc_root, 0, strlen($doc_root)-1);
    // Set the current script filename and absolute working directory
    $self_path_ar = explode('/', $doc_root_notrail . $_SERVER['SCRIPT_NAME']);
    $this->file_self = array_pop($self_path_ar);

    // if startdir was given, set it as CWD
    if (isset($startdir)) {
        if ( !file_exists($startdir) || !is_dir($startdir) ) {
            exit("<br>ERROR: Given starting directory '$startdir' is not a valid directory.<br>");
        }


        // if startdir begins with fwd slash, it's an absolute path
        $is_rel_path = (substr($startdir,0,1) !== '/');
        if ($is_rel_path) {
            $startdir = implode('/', $self_path_ar) . '/' . $startdir;
        }
      // set the supplied directory to current working dir and
      // resolve any symbolic links
      $this->cwd = realpath($startdir) . '/';
      // relative dir to the startdir is the top directory
      $this->cwrd = ''; // current working relative dir
    }
    else {
      $this->cwd = implode('/', $self_path_ar);
      // do some basic error checking
      if (!(file_exists($this->cwd) || is_dir($this->cwd) || is_link($this->cwd))) {
        exit("<b style=\"color:red;\">ERROR:</b> PHPhiler(): Unable to open directory '".$this->cwd."'<br>");
      }
      // resolve any symbolic links
      $this->cwd = realpath($this->cwd) . '/';
      // remove cwd from path to current script so we can get the relative dir
      $reldir = str_replace(strtolower(getcwd()), '', strtolower($this->cwd));
      // remove the first forward slash
      $reldir = substr($reldir, 1);
      // if it's the root dir, insert a './' instead of '/'
      $this->cwrd = ($reldir == '/' ? './' : $reldir); // current working relative dir
    }

  }


  // Main function that prints the entire HTML output. It is to be called from
  // any document that wants the table index printed.
  function index() {
    // get the current working dir
    $org_cwd = $this->cwd;

    // update the arrays
    $this->update($this->cwd, $this->getSubDirDescendLevels());

    // let's be good (ole') boys and change back to the dir we came from
    chdir($org_cwd);

    // check if the column we sort by is also shown or display an error
    if (!array_key_exists($this->__sortcolumn, $this->getColumnHeaders())) {
        exit ("<b>CONFIG ERROR:</b> The column to sort by is not included in the column headers.<br>");
    }



    $sortitem = NULL;
    // sort the arrays
    switch ($this->__sortorder) {
      case PHPH_ASC :
        switch ($this->__sortcolumn) {
          case PHPH_TITLE :
            if ($this->getDirectoriesOnTop()) {
              uasort($this->dirs, "strcasecmp");
              uasort($this->files, "strcasecmp");
              $sortitem = $this->dirs + $this->files;
            } else {
              $sortitem = $this->dirs + $this->files;
              uasort($sortitem, "strcasecmp");
            }
            break;
          case PHPH_SIZE :
            asort($this->filesizes, SORT_NUMERIC);
            $sortitem = &$this->filesizes;
            break;
          case PHPH_DATE :
            asort($this->filedates, SORT_NUMERIC);
            $sortitem = &$this->filedates;
            break;
          case PHPH_DESCR :
            uasort($this->filedescr, "strcasecmp");
            $sortitem = &$this->filedescr;
            break;
        }
        break;
      case PHPH_DESC :
        switch ($this->__sortcolumn) {
          case PHPH_TITLE :
            uasort($this->dirs, "strcasecmp");
            $this->dirs = array_reverse($this->dirs);
            uasort($this->files, "strcasecmp");
            $this->files = array_reverse($this->files);
            $sortitem = $this->dirs + $this->files;
            break;
          case PHPH_SIZE :
            arsort($this->filesizes, SORT_NUMERIC);
            $sortitem = &$this->filesizes;
            break;
          case PHPH_DATE :
            arsort($this->filedates, SORT_NUMERIC);
            $sortitem = &$this->filedates;
            break;
          case PHPH_DESCR :
            uasort($this->filedescr, "strcasecmp");
            $this->filedescr = array_reverse($this->filedescr);
            $sortitem = &$this->filedescr;
            break;
        }
        break;
    }

    $this->__total_entries = 0;

    if ($this->getIncludeSubDirectories()) {
      $this->__total_entries += sizeof($this->dirs);
    }
    if ($this->getIncludeFiles()) {
      $this->__total_entries += sizeof($this->files);
    }

    // set the table ID for the current listings
    $GLOBALS['PHPH_TABLE_ID']++;

    // if we want it all shown, set maxEntries to 0
    if ($_GET['m'] == 't' && $_GET['id'] == $GLOBALS['PHPH_TABLE_ID']) {
      $this->setMaxEntries(0);
    }
    // otherwise, slice a bit off the array and show only that
    elseif(isset($_GET['f']) && $_GET['id'] == $GLOBALS['PHPH_TABLE_ID']) {
      $sortitem = array_slice($sortitem, $_GET['f'], $this->getMaxEntries());
    }

    $this->setCurEntries(0);

    // create a HTML object to insert the arrays into and initialize it
    $html = new PHPhilerHTML($this);

    // if directories are to be shown first
    if ($this->getDirectoriesOnTop()) {

      // run through the directories first (if any exist)
      foreach($sortitem as $key => $elem) {
        // stop if we've reached the max number of entries
        if (($this->getMaxEntries() != 0) && ($this->getCurEntries() == $this->getMaxEntries())) break;
        if (array_key_exists ($key, $this->dirs)
            && $this->__include_sub_dirs) {
          $html->addDirToHtml($key, $this->dirs[$key]);
          $dir_exists = TRUE;
          $this->setCurEntries($this->getCurEntries()+1);
        }
      }
      // add a divider if there are still elements to show in the table
      if ($this->__total_entries > $this->getCurEntries()
          && $this->getCurEntries() != $this->getMaxEntries()
          && $dir_exists) {
        $html->addDividerToHtml(sizeof($this->getColumnHeaders()));
      }
      // then run through the files (if any exist)
      foreach($sortitem as $key => $elem) {
        // stop if we've reached the max number of entries
        if (($this->getMaxEntries() != 0) && ($this->getCurEntries() == $this->getMaxEntries())) break;
        if (array_key_exists ($key, $this->files)) {
          $html->addFileToHtml($key, $this->files[$key]);
          $this->setCurEntries($this->getCurEntries()+1);
        }
      }
    // else mingle directories and files
    } else {
      foreach($sortitem as $key => $elem) {
        // stop if we've reached the max number of entries
        if (($this->getMaxEntries() != 0) && ($this->getCurEntries() == $this->getMaxEntries())) break;
        if (array_key_exists ($key, $this->dirs)) {
          $html->addDirToHtml($key, $this->dirs[$key]);
        }
        elseif (array_key_exists ($key, $this->files)) {
          $html->addFileToHtml($key, $this->files[$key]);
        }
      }
    }

    // end the execution time calculation and add it to the HTML object
    $mtime = microtime();
    $mtime = explode(' ',$mtime);
    $mtime = $mtime[1] + $mtime[0];
    $endtime = $mtime;
    $totaltime = round($endtime - $this->starttime, 4);
    $html->addExecTimeToHtml($totaltime);

    // finally, print the HTML output to the document
    echo($html->getHtml());
  }



  // Function to validate dirs and filenames for the current dir.
  // It enters all info into the arrays.
  function update($curdir, $depth) {

    $this->cwd = $curdir;
    // change to proper directory
    chdir($this->cwd);

    //open the directory
    $dir_handle = @opendir($this->cwd) or exit("<b style=\"color:red;\">ERROR:</b> update(): Unable to open directory '$this->cwd'<br>");

    // current working file
    $file = NULL;
    // open each file/dir in the directory and retrieve its info
    while (($file = readdir($dir_handle)) !== false) {
      if ($file != '.' && $file != '..' // don't check dir aliases
         && !$this->isExcluded($file) ) // should it be excluded?
      {

        // if it's a file
        if (is_file($file)
            && $this->__include_files // do we want to include files?
            && $file !== $this->file_self // is it the script itself?
            && $file !== $this->templatefilename ) // is it a template?
        {
          $suf = $this->getSuffix($file);
          // check if we could get a suffix
          if ($suf == FALSE) continue;

          // get the actions for the current suffix
          $suf_actions = $this->getSuffixSettings($suf);

          // check if the suffix is valid
          if ($suf_actions == FALSE) continue;
          // then let's try to call each action in turn
          foreach ($suf_actions as $key => $action) {
            // if the function doesn't exist, try next action
            if (!function_exists($action)) continue;
            $fileinfo = call_user_func($action, $file);
            // if file info was retrieved then
            if ($fileinfo != FALSE) {
              // don't check the rest of the suffixes
              break;
            } else {
              // else go on to next action
              continue;
            }
          }
          // check for inconsistencies
          if ($fileinfo == FALSE) {
              exit ("<b style=\"color:red;\">ERROR</b>: file info retrieval (actions) failed for file '$file'!<br>");
          // if there is no title, use the dirname itself
          } elseif (!isset($fileinfo['title'])) {
              $fileinfo['title'] = stripSuffix($file);
          }
          // enter the title for the file into the array
          $this->files[$file] = $fileinfo['title'];
          // get the description, if available
          $this->filedescr[$file] = isset($fileinfo['description']) ? $fileinfo['description'] : $this->getNoDescriptionComment();
          if (array_key_exists(PHPH_SIZE, $this->getColumnHeaders())) {
            $this->filesizes[$file] = $this->readFileSize($file);
          }
          if (array_key_exists(PHPH_DATE, $this->getColumnHeaders())) {
            $this->filedates[$file] = readFileDate($file);
          }
          $this->filepaths[$file] = $this->cwd . $file;
          if ($fileinfo['uri'] == NULL) {
            $fileinfo['uri'] = '/' . str_replace ($this->getDocRoot(), '', $this->filepaths[$file]);
          }
          $this->fileuris[$file] = $fileinfo['uri'];

          // maybe create a template file (no need to do this for directories
          if ($this->getAction($this->filepaths[$file], 1) == TRUE) {
              $this->createTemplateFile();
          }
        }
        // if it's a directory and we want to include them in the listings
        elseif (is_dir($file)) {
          // ref to file for logical reasons
          $dir = &$file;
          // only enter dirinfo if directories are to be shown
          if ($this->getIncludeSubDirectories() === TRUE) {
            // get the actions for the current suffix
            $suf_actions = $this->getSuffixSettings(PHPH_DIR);

            // check if the suffix is valid
            if ($suf_actions == FALSE) continue;
            // then let's try to call each action in turn
            foreach ($suf_actions as $action) {
              // if the function doesn't exist, try next action
              if (!function_exists($action)) continue;
              $fileinfo = call_user_func($action, $dir);
              // if file info was retrieved then
              if ($fileinfo != FALSE) {
                // don't check the rest of the suffixes
                break;
              } else {
                // else go on to next action
                continue;
              }
            }

            // check for inconsistencies
            if ($fileinfo == FALSE) {
                exit ("<b style=\"color:red;\">ERROR</b>: dir info retrieval ('$action') failed completely for dir '$dir'!<br>");
            // if there is no title, use the dirname itself
            } elseif (!isset($fileinfo['title'])) {
                $fileinfo['title'] = $dir;
            }
            // enter the title for the file into the array
            $this->dirs[$dir] = $fileinfo['title'];
            // get the description, if available
            $this->filedescr[$dir] = isset($fileinfo['description']) ? $fileinfo['description'] : $this->getNoDescriptionComment();
            if (array_key_exists(PHPH_SIZE, $this->getColumnHeaders())) {
              $this->filesizes[$dir] = $this->readDirSize($dir);
            }
            if (array_key_exists(PHPH_DATE, $this->getColumnHeaders())) {
              $this->filedates[$dir] = readFileDate($dir);
            }
          $this->filepaths[$dir] = $this->cwd . $dir;
          if ($fileinfo['uri'] == NULL) {

            $fileinfo['uri'] = str_replace ($this->getDocRoot(), '', $this->filepaths[$dir]);
          }
            $this->fileuris[$dir] = $fileinfo['uri'];
          }

          // dir path must always be entered
          $this->filepaths[$dir] = $this->cwd . $this->cwrd . $dir . '/';
          // if we've the depth we wanted, or it's become negative
          // (in which case we started with 0 depth = infinite)
          if ($depth-1 > 0 || $depth-1 < 0) {
            $this->cwrd = $this->cwrd . $dir . '/';
            $this->cwrd = ($this->cwrd == '/' ? './' : $this->cwrd);
              $depth--;
              $this->update($this->cwd . $dir . '/', $depth);
              $depth++;
            $cwrd_ar = explode ('/', $this->cwrd);
            array_pop($cwrd_ar);
            array_pop($cwrd_ar);
            $this->cwrd = implode('/', $cwrd_ar) . '/';
            $this->cwrd = ($this->cwrd == '/' ? './' : $this->cwrd);
          }
          // when we return from the descent,
          // we need to pop the directory off the end
          chdir($curdir);
        }
      }
    }
    // closing dir apparently not possible.
    //@closedir($dir_handle) or exit('ERROR: createFileList(): Unable to close directory '.$this->cwd);

  }


  // Function that reads the size of a file or directory and formats the string
  function readFileSize($file) {
    if (is_file($file)) {
      // get the size of the file
      return filesize($file);
    } else {
      exit ("<b style=\"color:red;\">ERROR:</b> Could not open file '$file' to determine size<br>");
    }
  }


  // Function that reads the size of the contents of a directory.
  // Originally written by dmitri926 AT yahoo DOT com (snatched from php.net).
  // Modified for use with PHPhiler.
  function readDirSize($dir) {
    $totalsize = 0;
    if ($dirstream = opendir($dir)) {
      while (false !== ($filename = readdir($dirstream))) {
        $valid_suffix = $this->isValidSuffix($this->getSuffix($dir.'/'.$filename));
        if ($filename != '.'
            && $filename != '..'
            && $valid_suffix != FALSE) {
          if (is_file($dir.'/'.$filename))
            $totalsize += filesize($dir.'/'.$filename);
          if (is_dir($dir.'/'.$filename))
            $totalsize += $this->readDirSize($dir.'/'.$filename);
        }
      }
    closedir($dirstream);
    } else {
      exit ("<b style=\"color:red;\">ERROR:</b> Could not open dir '$file' to determine size<br>");
    }
    return $totalsize;
  }


  // Function that creates the template file for displaying custom header and footers.
  function createTemplateFile() {
    // define the path where we're going to put the template file
    $template_filename = $this->cwd.'/'.$this->templatefilename;

    // retrieve the full header path from the webserver and the config
    $header_path = $this->getDocRoot() . $this->getHTMLHeader();
    // attempt to open the header file
    $fh = @fopen($header_path, 'r') or exit("<b style=\"color:red;\">ERROR:</b> createTemplateFile(): Unable to open header file at '$header_path'<br>");
    // read the header file into a string
    $string .= fread($fh, filesize($header_path));
    // close the file again
    @fclose($fh);

    $string .= "\n<pre><div>\n";
    // append some php code to the string
    $string .= "\n<?php\n";
    $string .= 'if ($_GET[\'file\']) { echo htmlentities(file_get_contents(\''.$this->cwd.'\'.urldecode($_GET[\'file\']))); } ';
    $string .= "\n?>\n</div></pre>\n";

    // retrieve the full footer path from the webserver and the config
    $footer_path = $this->getDocRoot() . $this->getHTMLFooter();
      // attempt to open the footer file
    $fh = fopen($footer_path, 'r') or exit("<b style=\"color:red;\">ERROR:</b> createTemplateFile(): Unable to open footer file at '$footer_path'<br>");
    // read the footer file into a string
    $string .= fread($fh, filesize($footer_path));
    // close the file again
    fclose($fh);

    // delete the old template file if it exists
    if (file_exists($template_filename)) unlink($template_filename);
    // attempt to create the new template file
    $fh = @fopen($template_filename, 'a') or exit ("<b style=\"color:red;\">ERROR:</b> createTemplateFile(): Unable to create template file at '$template_filename'. Write permission denied.<br>");
    // write the string to the template file
    fwrite($fh, $string);
    // close the template file
    fclose($fh);
  }



  function getAction($file, $bitno) {
    $suf = $this->getSuffix($file);
    foreach ($this->getSuffixSettings() as $suf_act_ar) {
      if (strcasecmp($suf,$suf_act_ar[0]) == 0) {
        return $suf_act_ar[$bitno];
      }
    }
    // we should never get here:
    exit ("<b style=\"color:red;\">ERROR:</b> suffix '$suf' for file '$file' not defined!<br>");
  }

  // Function to return the suffix only
  function getSuffix($file) {
    //$file = translate_string($file, $GLOBALS['PHPH_PATH_FUNC'], $GLOBALS['PHPH_PATH_ARGS']);
    if (is_file($file)) {
      $pathinfo = pathinfo($file);
      if ($pathinfo['extension'] !== '') {
        return strtolower($pathinfo['extension']);
      }
    } elseif (is_dir($file)) {
        return PHPH_DIR; // special case keyword
    }
    exit ("<b style=\"color:red;\">ERROR:</b> Could not determine suffix for file '$file' in dir '".$this->cwd."'<br>");
  }

  // _______________________________________________________________________ //
  // Set and get functions
  // _______________________________________________________________________ //

  // suffix action array
  function setSuffixSettings() {
    $suf_ar = func_get_args();
    $GLOBALS['PHPH_SUFFIXES'] = array();
    $suffixes = &$GLOBALS['PHPH_SUFFIXES'];
    foreach($suf_ar as $suf_act) {
      if (!is_array($suf_act)) {
        exit ("<b style=\"color:red;\">ERROR:</b> Config: '$suf_act' is not an array.<br>");
      }
      $suffixes[] = $suf_act[0];
    }
    $GLOBALS['PHPH_SUFFIX_SETTINGS'] = $suf_ar;
  }

  function getSuffixSettings($suf=NULL) {
    if ($suf == NULL) {
      return $GLOBALS['PHPH_SUFFIX_SETTINGS'];
    } else {
      foreach ($GLOBALS['PHPH_SUFFIX_SETTINGS'] as $suf_act_ar) {
        if (strcasecmp($suf,$suf_act_ar[0]) == 0) {
          $suf_actions = array();
          for ($i=3; $i < sizeof($suf_act_ar); $i++) {
            $suf_actions[] = $suf_act_ar[$i];
          }
          // enter the default action into the end of the array
          $suf_actions[] = 'PHPH_filename';
          return ($suf_actions);
        }
      }
      return FALSE;
    }
  }

  function isValidSuffix ($suf) {
    return array_search($suf, $GLOBALS['PHPH_SUFFIXES']);
  }

  function isExcluded($file) {
    if (in_array ($file,$this->getExcludedElements()))
      return TRUE;

    foreach ($this->getWildcardElements() as $wildcard) {
      $split = strpos($wildcard, '*');
      $prefixToSearch = quotemeta (substr($wildcard, 0, $split));
      $suffixToSearch = quotemeta (substr($wildcard, $split+1, strlen($wildcard)));

      $regexp = $prefixToSearch . "[\d\s\w]*" . $suffixToSearch;
      if (eregi($regexp, $file)) {
        return TRUE;
      }
    }
    return FALSE;
  }

  function setURITranslationFunc($func=NULL, $arg_ar=NULL) {
      if ( !isset($arg_ar) && !isset($func) ) {
          $arg_ar = array(ENT_COMPAT, $this->getCharset());
          $func = 'htmlentities';
      } else {
          echo "CONFIG: setURITranslationFunc missing arguments!<br>";
      }
      $GLOBALS['PHPH_URI_FUNC'] = $func;
      $GLOBALS['PHPH_URI_ARGS'] = $arg_ar;
  }

  function setPathTranslationFunc($func=NULL, $arg_ar=NULL) {
      $GLOBALS['PHPH_PATH_FUNC'] = $func;
      $GLOBALS['PHPH_PATH_ARGS'] = $arg_ar;
  }

  function setSortOrder($column, $order) {
    $this->__sortcolumn = $column;
    $this->__sortorder = $order;
  }

  function setDateFormat($string) {
    $GLOBALS['PHPH_DATE_FORMAT'] = $string;
  }

  function getDateFormat() {
    return $GLOBALS['PHPH_DATE_FORMAT'];
  }

  // comment if item not descriptied (string)
  function setNoDescriptionComment($string) {
    $GLOBALS['PHPH_NODESC_COMMENT'] = $string;
  }
  function getNoDescriptionComment() {
    return $GLOBALS['PHPH_NODESC_COMMENT'];
  }

  function setTitleTag($string) {
    $this->__title_tag      = '<'.$string.'>';
    $this->__title_end_tag  = '</'.$string.'>';
  }

  function setDescTag($string) {
    $this->__desc_tag       = '<'.$string.'>';
    $this->__desc_end_tag   = '</'.$string.'>';
  }

  function setIncludeSubDirectories($bool) {
    $this->__include_sub_dirs = $bool;
  }
  function getIncludeSubDirectories() {
    return $this->__include_sub_dirs;
  }

  function setIncludeFiles($bool) {
    $this->__include_files = $bool;
  }
  function getIncludeFiles() {
    return $this->__include_files;
  }

  function setSubDirDescendLevels($num) {
    $GLOBALS['PHPH_SUBDIR_LEVELS'] = $num;
  }
  function getSubDirDescendLevels() {
    return $GLOBALS['PHPH_SUBDIR_LEVELS'];
  }

  function setIncludeTableHeader($bool) {
    $this->__include_table_header = $bool;
  }

  function setIncludeTableFooter($bool) {
    $this->__include_table_footer = $bool;
  }

  function setColumnHeaders($col_ar) {
    if (!is_array($col_ar)) exit("<b style=\"color:red;\">ERROR:</b> Config: '$col_ar' is not an array.<br>");
    $GLOBALS['PHPH_COL_HEADERS'] = $col_ar;
  }
  function getColumnHeaders() {
    return $GLOBALS['PHPH_COL_HEADERS'];
  }

  // dirs on top? (bool)
  function setDirectoriesOnTop($bool) {
    $GLOBALS['PHPH_DIRS_ON_TOP'] = $bool;
  }
  function getDirectoriesOnTop() {
    return $GLOBALS['PHPH_DIRS_ON_TOP'];
  }

  // maximum file/directory entries in a list (int)
  function setMaxEntries($num) {
    $GLOBALS['PHPH_MAX_ENTRIES'] = $num;
  }
  function getMaxEntries() {
    return $GLOBALS['PHPH_MAX_ENTRIES'];
  }

  function setCurEntries($num) {
    $GLOBALS['PHPH_CUR_ENTRIES'] = $num;
  }
  function getCurEntries() {
    return $GLOBALS['PHPH_CUR_ENTRIES'];
  }

  function setIncludeNavigation($bool) {
    $GLOBALS['PHPH_INCLUDE_NAVIGATION'] = $bool;
  }
  function getIncludeNavigation() {
    return $GLOBALS['PHPH_INCLUDE_NAVIGATION'];
  }

  function setMoreLinkMessages($msg_ar) {
    $GLOBALS['PHPH_MORE_LINK_MESSAGES'] = $msg_ar;
  }
  function getMoreLinkMessages() {
    return $GLOBALS['PHPH_MORE_LINK_MESSAGES'];
  }


  function setMaxPageLinks($int) {
    $GLOBALS['PHPH_MAX_PAGE_LINKS'] = $int;
  }
  function getMaxPageLinks() {
    return $GLOBALS['PHPH_MAX_PAGE_LINKS'];
  }

  function setExcludedElements($excl_ar) {
    foreach ($excl_ar as $excl) {
      if (stristr($excl, '*') == TRUE) {
        $wc_ar[] = $excl;
      } else {
        $ex_ar[] = $excl;
      }
    }
    $GLOBALS['PHPH_EXCLUDED_ELEMENTS'] = $ex_ar;
    $GLOBALS['PHPH_WILDCARD_ELEMENTS'] = $wc_ar;
  }

  function getExcludedElements() {
    return $GLOBALS['PHPH_EXCLUDED_ELEMENTS'];
  }

  function getWildcardElements() {
    return $GLOBALS['PHPH_WILDCARD_ELEMENTS'];
  }


  function setCSSsuffix($string) {
    if ($string != '')
      $GLOBALS['PHPH_CSS_SUFFIX'] = '-' . $string;
  }
  function getCSSsuffix() {
    return $GLOBALS['PHPH_CSS_SUFFIX'];
  }

  function setCharset($string=NULL) {
      if (($string == NULL) || empty($string)) {
          $string = 'ISO-8859-1';
      }
      $GLOBALS['PHPH_CHARSET'] = $string;
  }
  function getCharset() {
    return $GLOBALS['PHPH_CHARSET'];
  }

  function setDocRoot($string) {
    $GLOBALS['PHPH_DOC_ROOT'] = $string;
  }

  function getDocRoot() {
    return $GLOBALS['PHPH_DOC_ROOT'];
  }

  // Set the complete absolute path to the PHPhiler source files.
  function setPHPhilerPath($string) {
    $GLOBALS['PHPH_PATH'] = $string;
  }
  function getPHPhilerPath() {
    return $GLOBALS['PHPH_PATH'];
  }

  function setHTMLHeader($string) {
    $this->__header_path = $string;
  }

  function getHTMLHeader() {
    return $this->__header_path;
  }

  function setHTMLFooter($string) {
    $this->__footer_path = $string;
  }

  function getHTMLFooter() {
    return $this->__footer_path;
  }

  function setStatusbarFileComment($string) {
    $this->__statusbar_comment = $string;
  }

} // end of class PHPhiler

?>
