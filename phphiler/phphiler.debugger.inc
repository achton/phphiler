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
        $stringout = '<html><head><title>Debugger</title></head><body><code>';
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

        $stringout = NULL;

        // check the error_reporting level
        switch($int) {
            // just a notice, nothing important
            case E_NOTICE :
                $stringout .= '<span>';
                $stringout .= '<b>NOTICE:</b>';
                break;
            // a warning, this is serious
            case E_WARNING :
                $stringout .= '<span style="color:red;">';
                $stringout .= '<b>WARNING:</b>';
                break;
            // an error, fix it now!
            case E_ERROR :
                $stringout .= '<span style="color:red;">';
                $stringout .= '<b><u>ERROR:</b>';
                break;
            default :
                $stringout .= '<span>';

        }

        $stringout .= '[f ' . $function . '] [l ' . $line . ']&nbsp;&nbsp;' . $string . "\n" . '<br>';
        $stringout .= '</span>' . "\n" . '<br>';


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
        $stringout = '<br></code></body></html>';
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