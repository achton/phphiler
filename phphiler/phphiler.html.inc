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

// This class is for creating PHPhilerHTML objects that contain the HTML
// code for displaying the tables.
class PHPhilerHTML {

  var $__html                 = NULL; // the main class variable (string)
  var $__exec_string          = NULL; // the execution time string (string)
  var $__phphiler             = NULL; // reference to the PHPhiler object

  // Class constructor.
  function PHPhilerHTML(&$PHPhiler) {

    // keep a reference to the PHPhiler object in a class variable
    $this->__phphiler = &$PHPhiler;

    $this->addStringToHtml('<div class="phphiler'
                          . $this->__phphiler->getCSSsuffix() . '">' . "\n"
                          . '<table class="phphiler-table'
                          . $this->__phphiler->getCSSsuffix() .'" '
                          . 'id="t' . $GLOBALS['PHPH_TABLE_ID'] . '">' . "\n");

    if ($this->__phphiler->__include_table_header) {
      $string .= '<tr>' . "\n";
      foreach ($this->__phphiler->getColumnHeaders() as $col => $colname) {
        $header = $this->__phphiler->getColumnHeaders();
        $string .= '<td class="phphiler-header'
                  . $this->__phphiler->getCSSsuffix() .'">' . "\n"
                  . htmlentities($header[$col])
                  .'</td>' . "\n";
      }
      $string .= '</tr>' . "\n";
      $this->addStringToHtml($string);
    }
  }

  // Function that returns the main HTML string class variable.
  function getHtml() {
    $msg_ar = $this->__phphiler->getMoreLinkMessages();
    $colspan = sizeof($this->__phphiler->getColumnHeaders());

    $total_rows = $this->__phphiler->__total_entries; // total files/dirs
    $cur_rows = $this->__phphiler->getCurEntries(); // number of rows shown right now
    $max_rows = $this->__phphiler->getMaxEntries(); // rows to show at a a time
    if ($max_rows > 0) {
      $max_pages = $this->__phphiler->getMaxPageLinks(); // max page links to show
      $total_pages = floor($total_rows / $max_rows) + ($total_rows % $max_rows != 0 ? 1 : 0);
      $last_row_num = $total_rows - ($total_rows % $max_rows);
      // only get the row number for the current table
      if ($_GET['id'] == $GLOBALS['PHPH_TABLE_ID'])
        $cur_row_num = $_GET['f'];

      $cur_page = floor($cur_row_num / $max_rows);


      // if we want to show more pages than there are from the current page and up
      if ($cur_page+$max_pages > $total_pages) {
        // start at first available page (ie. totalpages-maxpages) or zero
        $sum = $total_pages-$max_pages;
        $start = ($sum >= 0 ? $sum : 0);
        // and end at last page
        $end = $total_pages-1;
      } else {
        // else start at current page
        $start = $cur_page;
        // and end at the wanted maximum (ie. currentpage+maxpages)
        $end = $cur_page+$max_pages-1;
      }
  //echo "start: $start <br>";
  //echo "end: $end <br>";

      for ($page=$start; $page <= $end; $page++) {
        $row_num = $page * $max_rows;
  //echo "row_num: $row_num <br>";
  //echo "page: $page <br>";
        if ($page == ($cur_page)) {
          $prev_row_num = ($page-1) * $max_rows;
          $next_row_num = ($page+1) * $max_rows;

          $pages_html .= '<strong title="' . $msg_ar[2] . ' '. ($cur_page+1) .'" style="color:#a22;">' . ($page+1) . '</strong>&nbsp;';

        } else {
          $pages_html .= '<a href="?id=' . $GLOBALS['PHPH_TABLE_ID']
                       . '&m=f&f=' . ($row_num)
                       . '#t' . $GLOBALS['PHPH_TABLE_ID'] . '" title="' . $msg_ar[3] . ' '.($page+1).'">'
                       . ($page+1) . '</a>&nbsp;';
        }
      }

      // make sure we don't display an empty table
      // if the total number of rows divided by maxrows equals exact chunks
      if ($total_rows % $max_rows == 0) {
          $last_row_num = $total_rows-$max_pages;
      }

  //echo "<br>";
  //echo "total_rows: ". $total_rows ."<br>";
  //echo "cur_rows: ". $cur_rows ."<br>";
  //echo "max_rows: ". $max_rows ."<br>";
  //echo "<br>";
  //echo "prev_row_num: " . $prev_row_num . "<br>";
  //echo "cur_row_num: " . $cur_row_num . "<br>";
  //echo "next_row_num: " . $next_row_num . "<br>";
  //echo "last_row_num: " . $last_row_num . "<br>";
  //echo "<br>";
  //echo "total_pages: " . $total_pages . "<br>";
  //echo "cur_page: " . $cur_page . "<br>";
  //echo "max_pages: " . $max_pages . "<br>";


      $show_all_html = '<a href="?id=' . $GLOBALS['PHPH_TABLE_ID']
                    . '&m=t'
                     . '#t' . $GLOBALS['PHPH_TABLE_ID'] . '">'
                     . $msg_ar[0] . ' ('.$total_rows.')</a>' . "\n";

      $back_to_first_html = ($prev_row_num >= 0 ? '<a href="?id=' . $GLOBALS['PHPH_TABLE_ID']
                     . '&m=f&f=0'
                     . '#t' . $GLOBALS['PHPH_TABLE_ID'] . '" title="' . $msg_ar[4] . '">'
                     . '&laquo;</a>&nbsp;' : NULL);
      //'<a href="#" title="Go to first page">&laquo;</a>&nbsp;';
      $back_one_html = ($prev_row_num >= 0 ? '<a href="?id=' . $GLOBALS['PHPH_TABLE_ID']
                     . '&m=f&f=' . $prev_row_num
                     . '#t' . $GLOBALS['PHPH_TABLE_ID'] . '" title="' . $msg_ar[3] . ' '.($prev_row_num / $max_rows + 1).'">'
                     . '&lsaquo;</a>&nbsp;' : NULL);
      $fwd_one_html = ($next_row_num <= $last_row_num && $next_row_num < $total_rows ?
                       '<a href="?id=' . $GLOBALS['PHPH_TABLE_ID']
                     . '&m=f&f=' . $next_row_num
                     . '#t' . $GLOBALS['PHPH_TABLE_ID'] . '" title="' . $msg_ar[3] . ' '.($next_row_num / $max_rows + 1).'">'
                     . '&rsaquo;</a>&nbsp;' : NULL);
      $fwd_to_last_html = ($next_row_num <= $last_row_num && $next_row_num < $total_rows ?
                       '<a href="?id=' . $GLOBALS['PHPH_TABLE_ID']
                     . '&m=f&f=' . $last_row_num
                     . '#t' . $GLOBALS['PHPH_TABLE_ID'] . '" title="' . $msg_ar[5] . '">'
                     . '&raquo;</a>&nbsp;' : NULL);

      if ($this->__phphiler->getIncludeNavigation()) {
        if ($cur_rows < $total_rows && $cur_rows <= $max_rows) {
          $this->addStringToHtml('<tr><td class="phphiler-navigation'
                                . $this->__phphiler->getCSSsuffix() . '" '
                                . 'colspan="'.$colspan.'">' . "\n"
                                . '<div style="float:left;">'
                                . $show_all_html
                                . '</div>'
                                . '<div style="float:right;">'
                                . $back_to_first_html
                                . $back_one_html
                                . $pages_html
                                . $fwd_one_html
                                . $fwd_to_last_html
                                . '</div>'
                                . '</td></tr>' . "\n");
          //$GLOBALS['max_temp'] = TRUE;
        } elseif ($cur_rows == $total_rows && $max_rows == 0 && $_GET['m'] == 't') {
            $this->addStringToHtml('<tr><td class="phphiler-navigation'
                                  . $this->__phphiler->getCSSsuffix() . '" '
                                  . 'colspan="'.$colspan.'">' . "\n"
                                  . '<a href="?id=' . $GLOBALS['PHPH_TABLE_ID']
                                  . '&m=f'
                                  . '#' . 't'.$GLOBALS['PHPH_TABLE_ID'] . '">' . "\n"
                                  . $msg_ar[1] . '</a>'
                                  . '</td></tr>' . "\n");
        }
      }
    }
    if ($this->__phphiler->__include_table_footer) {
      $this->addStringToHtml($this->getHtmlFooter());
    }
    $this->addStringToHtml('</table></div>' . "\n");
//echo "<b>HTML</b>: returning HTML<br>";
    return $this->__html;
  }

  // Function that adds a directory and its info to the HTML table.
  function addDirToHtml($dir, $title) {
//echo "<b>HTML</b>: adding dir '$dir'<br>";
    // increase the class row counter so we know how many rows have been added
    // get the custom datestring
    $datestr = $this->__phphiler->getDateFormat();

    $dirinfo = array (PHPH_TITLE => $title,
                      PHPH_DESCR => $this->__phphiler->filedescr[$dir],
                      PHPH_DATE  => date($datestr, $this->__phphiler->filedates[$dir]),
                      PHPH_SIZE  => roundSize($this->__phphiler->filesizes[$dir]));
    $action = $this->__phphiler->fileactions[$dir];

    $statusbartext = $this->getStatusbar($dir, $title);

//echo "<b>HTML</b>: URI for dir '$dir' is '".$this->__phphiler->fileuris[$dir]."' <br>";

    //
    $link_string = '/' . translate_string($this->__phphiler->fileuris[$dir],
                                    $GLOBALS['PHPH_PATH_FUNC'],
                                    $GLOBALS['PHPH_PATH_ARGS']);

//echo "<b>HTML</b>: Link for dir '$dir' is '".$link_string."' <br>";

    // set the string and add all the parameters to the HTML row
    $string .= '<tr class="phphiler-dir-rows' . $this->__phphiler->getCSSsuffix() .'">';

    // write out the columns in requested order
    foreach ($this->__phphiler->getColumnHeaders() as $col => $colname) {
      switch ($col) {
         // title is a special case (ha ha)
        case 'title':
          $string .= '<td class="phphiler-'.PHPH_TITLE.'-cols'
                    . $this->__phphiler->getCSSsuffix() . '">' . "\n"
                    . '<a href="' .$link_string.'" ' . $statusbartext.'>' . "\n"
                    . htmlentities($dirinfo[PHPH_TITLE], ENT_COMPAT, $this->__phphiler->getCharset())
                    . '</a></td>' . "\n";
          break;
        // the other columns all need the same HTML code
        default :
          $string .= '<td class="phphiler-'.strtolower($col).'-cols'
                    . $this->__phphiler->getCSSsuffix() .'">' . "\n"
                    . $dirinfo[$col]
                    . '</td>' . "\n";
      }
    }
    $string .= '</tr>';
    // add the string to the main class variable
    $this->addStringToHtml($string);
  }

  // Function that adds a divider row to the HTML table.
  function addDividerToHtml($colspan) {
    $string = '<tr><td colspan="' .$colspan .'" '
            . 'class="phphiler-divider' . $this->__phphiler->getCSSsuffix() .'">' . "\n"
            . '</td></tr>' . "\n";

    // add the string to the main class variable
    $this->addStringToHtml($string);
  }


  // Function that adds a file and its info to the HTML table.
  function addFileToHtml($file, $title) {
//echo "<b>HTML</b>: adding file '$file with title '$title'<br>";
    // increase the class row counter so we know how many rows have been added
    //$this->__cur_rows++;
    // get the custom datestring
    $datestr = $this->__phphiler->getDateFormat();

    $fileinfo = array(PHPH_TITLE => $title,
                      PHPH_DESCR => $this->__phphiler->filedescr[$file],
                      PHPH_DATE => date($datestr, $this->__phphiler->filedates[$file]),
                      PHPH_SIZE => roundSize($this->__phphiler->filesizes[$file]));

    // now get the real path for the file
    //$file = $this->__phphiler->filepaths[$file];

    // define the string that may contain a 'target' entry for the link (depends on config)
    $target_string = '';
    // check the corresponding action bit of the file
    if ($this->__phphiler->getAction($this->__phphiler->filepaths[$file], 2) == TRUE) {
      $target_string = 'target="_blank"';
    }

    // define the string that contains the link to the file
    $link_string = "";
    // check if the action bit for header/footer encapsulation is set
    if ($this->__phphiler->getAction($this->__phphiler->filepaths[$file], 1) == TRUE) {
      // if it's set, use the template file
      $link_string = $this->__phphiler->templatefilename.'?file='.($file);
    // check if a uri for the file has been given
    } elseif (!empty($this->__phphiler->fileuris[$file])) {
      $link_string = $this->__phphiler->fileuris[$file];
    // else link directly to the file
    } else {
      $link_string = translate_string($this->__phphiler->filepaths[$file],
                                      $GLOBALS['PHPH_PATH_FUNC'],
                                      $GLOBALS['PHPH_PATH_ARGS']);
    }
    $link_string = '/' . $link_string;
//echo "<b>HTML</b>: URI for file '$file' is '".$this->__phphiler->fileuris[$file]."' <br>";

    // set the contents of the statusbar when the link is hovered
    $statusbartext = $this->getStatusbar($file, $fileinfo[PHPH_TITLE]);

    // set the main HTML string to add to the table
    $string = '<tr class="phphiler-file-rows' . $this->__phphiler->getCSSsuffix() .'">';

    foreach ($this->__phphiler->getColumnHeaders() as $col => $colname) {
      switch ($col) {
         // title is a special case (ha ha)
        case 'title':
          // translate the title string
          $title = translate_string($fileinfo['title'], $GLOBALS['PHPH_URI_FUNC'], $GLOBALS['PHPH_URI_ARGS']);
          $string .= '<td class="phphiler-'.PHPH_TITLE.'-cols'
                    . $this->__phphiler->getCSSsuffix() .'">' . "\n"
                    . '<a href="' . $link_string.'" '.$target_string . $statusbartext.'>' . "\n"
                    . $title
                    . '</a></td>' . "\n";
           break;
         // the other columns all need the same code
         default :
          $string .= '<td class="phphiler-' . strtolower($col) .'-cols' . $this->__phphiler->getCSSsuffix() .'">' . "\n"
                    . $fileinfo[$col]
                    . '</td>' . "\n";
      }
    }
    $string .= '</tr>' . "\n";
    // add the string to the main class variable
    $this->addStringToHtml($string);
  }


  // Function that returns the HTML footer to add to table listings.
  function getHtmlFooter() {
    $string .= '<tr><td colspan="'. sizeof($this->__phphiler->getColumnHeaders()) .'" '
              . 'class="phphiler-footer"> Indexed with '
              . '<a href="' . PHPHILER_LINK . '">PHPHiL&#931;R</a> ' . "\n"
              . PHPHILER_VER . ' &copy; 2003-2004. ' . $this->__exec_string
              . '</td></tr>' . "\n";
    return $string;
  }


  //
  function getStatusbar($file, $title) {
    $comment = NULL;
    switch (strtolower($this->__phphiler->__statusbar_comment)) {
      case 'link' :
        // don't insert any javascript, just let the link be shown
        return NULL;
      case 'filename' :
        $comment = $file;
        break;
      case 'title' :
        $comment = $title;
        break;
      default :
        $comment = $this->__statusbar_comment;
    }
    $string = 'onMouseOver="window.defaultStatus=\' \'; window.status=\''
              .$comment.'\'; return true"';
    return $string;
  }

  // Function that sets the execution time string class variable.
  function addExecTimeToHtml($time) {
    $this->__exec_string = 'Indexing took '.$time.'s.';
  }

  // Function that appends a string to the main HTML string class variable.
  function addStringToHtml($string) {
    $this->__html .= $string;
  }

} // end of class PHPhilerHTML

?>
