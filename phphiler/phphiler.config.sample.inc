<?php
// PHPhiler sample configuration file
// PHPhiler is copyright (c) 2003-2004 Achton N. Netherclift

// ________________________________________________________________________
//
// File paths and related
// ________________________________________________________________________

// The absolute path to the document root of your webserver
// Defaults to $DOCUMENT_ROOT. Remember the trailing slash.
//$this->setDocRoot('/var/www/');

// Path to the directory containing the PHPhiler source files, relative to
// setDocRoot(). Remember the trailing slash.
$this->setPHPhilerPath('include/phphiler/');

// Where to find the HTML header.
$this->setHTMLHeader('include/header.inc');

// Where to find the HTML footer.
$this->setHTMLFooter('include/footer.inc');


// ________________________________________________________________________
//
// Configuring directory lists
// ________________________________________________________________________

// Sets whether files should be shown in the file list.
$this->setIncludeFiles(true);

// Sets whether subdirectories should be shown in the file list.
$this->setIncludeSubDirectories(true);

// Set the number of subdirectories to descend into when generating the
// lists. 0 = unlimited, 1 = current dir only, 2 = one subdirectory level,
// etc.
$this->setSubDirDescendLevels(1);

// Set files and/or directories that are to be excluded from the file list.
// Use an asterisk (*) to match files prefixed by the same string (e.g.
// '~$temp*').
$this->setExcludedElements( array('testdir',
                                  'index.html',
                                  '~$$*',
                           ));

// Set the maximum file and/or directory entries that are shown at any one
// time in a file listing. 0 means no limit.
$this->setMaxEntries(0);


// ________________________________________________________________________
//
// Displaying directory lists
// ________________________________________________________________________

// Whether to always display directories on top (otherwise mingle with
// files).
$this->setDirectoriesOnTop(true);

// Whether to include a header with column titles in the table.
$this->setIncludeTableHeader(true);

// Whether to include a footer in the table.
$this->setIncludeTableFooter(true);

// Set the column to sort the file lists by, and the sort order.
$this->setSortOrder(PHPH_SIZE, PHPH_DESC);

// Set whether to display a navigation menu at the bottom of file lists. It
// is only displayed if there are more elements than set in MaxEntries().
$this->setIncludeNavigation(true);

// Set the maximum number of page links to be shown at a time in the
// navigation menu (if enabled).
$this->setMaxPageLinks(3);

// Add a suffix to all CSS selectors that relate to the current file list.
$this->setCSSsuffix('');

// Set which charset to interpret the file- and directorynames with.
// Available charsets are listed at http://php.net/htmlentities/.
$this->setCharset('ISO-8859-1');

// ________________________________________________________________________
//
// Settings
// ________________________________________________________________________


// This determines the settings for individual file types, based on their
// file suffix. File suffixes that are not included here will NOT be shown
// in the file list.
//
// Each array MUST contain the following elements: suffix, boolean1,
// boolean2. After these elements, it may optionally contain the name of
// one or more actions to perform for this file type.

// The suffix is a string containing a file suffix, excluding punctuation.
// The first boolean value determines whether to wrap a custom header and
// footer around this file type
// The second boolean value determines whether to open the file in a new
// The name of actions to perform for this file type can be entered last
// in the array. If no action is given, the default PHP_filename action is
// used.
$this->setSuffixSettings( array( PHPH_DIR, FALSE, FALSE ),
                          array( 'txt', TRUE, FALSE ),
                          array( 'html', FALSE, TRUE, 'PHPH_NFOfile' ),
                          array( 'mp3', FALSE, TRUE, 'MyAction', 'PHPH_NFOfile' ),
                         );

// This setting determines which function will translate filenames into
// HTML (useful for filenames with extended characters).  The first
// argument is the name of the function that is to be used for translating
// the filenames. If the function needs any parameters, these must be
// presented in an array, which is the second argument for this setting.
$this->setURITranslationFunc();

// This setting is used to translate strings that will be used by PHP in
// the filesystem. Works just like setURITranslationFunc() above.
$this->setPathTranslationFunc();


// ________________________________________________________________________
//
// Messages and text
// ________________________________________________________________________

// This is the description to print if a file or directory is not
// descripted.
$this->setNoDescriptionComment('This item has not yet been descripted.');

// This sets the text to display in the browser's status bar when hovering
// over files with the mouse. The current keywords are:
//      Link : the actual link (standard browser behaviour)
//      Filename : just the filename
//      Title : the title of the file
// If you write any other string, it will be displayed for every file
// listed. Keywords are case-insensitive.
$this->setStatusbarFileComment('Link');

// This setting sets the various messages that appear when hovering over the
// links in the bottom navigation menu (if enabled).
$this->setMoreLinkMessages( array(
                                  'Show all',           // Show all files
                                  'Show less',          // Show less files
                                  'You are on page',    // You are on page
                                  'Go to page',         // Go to page
                                  'Go to first page',   // Go to first page
                                  'Go to last page',    // Go to last page
                           ));

// Set the format for the date column in accordance with
// http://php.net/date/.
$this->setDateFormat("jS M Y");

// This array sets the columns to display, in which order to display them,
// and what to name the column headers.
$this->setColumnHeaders( array(PHPH_TITLE => 'title',
                               PHPH_SIZE  => 'size',
                               PHPH_DATE  => 'date',
                               PHPH_DESCR => 'description',
                        ));


// ________________________________________________________________________
//
// End of file phphiler.config.sample.inc
// ________________________________________________________________________

// $Id$
?>
