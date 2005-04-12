<?php
/**
 * A class that creates a debugging window for PHPhiler containing debug information from runtime.
 *
 * @package     PHPhiler
 * @version     $Id$
 * @author      Achton N. Netherclift <achton@netherclift.net>
 * @access      private
 */
class PHPhiler_Debugger {

    /**
     * Path to the debug log.
     * @var string
     */
    var $LogPath = NULL;

    /**
     * Name of the debug log.
     * @var string
     */
    var $LogFileName = 'phphiler.debug.html';

    /**
    * Constructor that sets the path to the debug log and initializes it.
    *
    * @param    string  $path    The path to the debug log
    * @return   void
    */
    function PHPhiler_Debugger($path) {
        // set the path to the debug log
        $this->LogPath = $path;

        // do some errorchecking
        if (!file_exists($this->LogPath)) {
            exit('ERROR: PHPhiler_Debugger(): Couldn\'t find path to debug log: ' . $this->LogPath);
        } elseif(!is_dir($this->LogPath)) {
            exit('ERROR: PHPhiler_Debugger(): Path is not a directory: ' . $this->LogPath);
        } elseif(!is_writable($this->LogPath)) {
            exit('ERROR: PHPhiler_Debugger(): Directory is not a writable: ' . $this->LogPath);
        }

        // delete the old logfile
        unlink($this->LogPath . $this->LogFileName);
        // attempt to create a new file
        $fh = @fopen($this->LogPath . $this->LogFileName, 'a') or exit('ERROR: PHPhiler_Debugger(): Couldn\'t create debug log file.');

        // write a header to the debug log
        $stringout = <<<END
        <html>
        <head>
        <title>Debugger</title>
        <style type="text/css">
        body {
            background-color : #eee;
            font-family : verdana;
            font-size : 11px;
        }
        body div {
            border : 1px solid #aaa;
            margin : 2px;
            margin-bottom : 3px;
            padding : 2px;
        }
        .notice, .warning, .error {
            font-weight : bold;
        }
        .notice {
            background-color : #ffff99;
        }
        .warning {
            background-color : #ffff99;
        }
        .error {
            text-decoration : underline;
            background-color : #ffff99;
        }

        </style>
        </head>
        <body>
END;
        fwrite($fh, $stringout);
        fclose($fh);

    }

    /**
    * Adds a string to the debug file, consisting of the type of error message,
    * name of the calling function and a description.
    *
    * @param    int     $int            The error level of the message (E_NOTICE, E_WARNING, E_ERROR)
    * @param    string  $string         Description of the error.
    * @param    string  $function       Name of the calling function.
    * @param    mixed   $line           The line number
    * @return   void
    */
    function toDebugger($int, $string, $function=NULL, $line=NULL, $file=NULL) {

        $filename = substr($file, strrpos($file, '/')+1);

        $stringout = NULL;
        $e_level = NULL;

        // check the error_reporting level and set some HTML accordingly
        switch($int) {
            // just a notice, nothing important
            case E_NOTICE :
                $e_level = '<div class="notice">NOTICE</div>';
                break;
            // a warning, this is serious
            case E_WARNING :
                $e_level = '<div class="warning">WARNING</div>';
                break;
            // an error, fix it now!
            case E_ERROR :
                $e_level = '<div class="error">ERROR</div>';
                break;
            default :
            //
        }

        $filename_html = (empty($filename) ? NULL : 'file <b><code>' . $filename .  '</code></b> | ');
        $line_html = 'line <b><code>' . $line .  '</code></b> | ';
        // check if magic constant __FUNCTION__ is supported (PHP >= 4.3.0)
        $function_html = ($function == '__FUNCTION__' ? NULL : 'func <b><code>' . $function . '</code></b> | ');

        $stringout .= '<div>';
        $stringout .= $e_level;
        $stringout .= $filename_html . $line_html . $function_html . '<br>' . "\n";
        $stringout .= $string . '<br>' . "\n";
        $stringout .= '</div>';

        // write to the debug log and close it
        $fh = fopen($this->LogPath . $this->LogFileName, 'a');
        fwrite($fh, $stringout);
        fclose($fh);
    }


    /**
    * Opens the debug window. It writes some javascript to the current document,
    * thus opening a new browser window containing the debug log.
    *
    * @return void
    */
    function openDebugger($LogURI) {

        // open the file in append mode
        $fh = @fopen($this->LogPath . $this->LogFileName, 'a');

        // write a HTML footer to the debug log and close it
        $stringout = '</body></html>';
        fwrite($fh, $stringout);
        fclose($fh);

        // write some JS to current document to open the debug log
        echo <<<END
        <script type="text/javascript">
        config ='toolbar=no, location=no, directories=no, status=no, menubar=no, width=800, resizable=yes, scrollbars=yes';
        output = window.open ("
END;
        echo $LogURI . $this->LogFileName;
        echo '","outputwindow", config);</script>';
    }

}

?>