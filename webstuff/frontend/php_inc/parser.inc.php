<?
class Parser{	
  var $contentArray;
  var $ts = 0;

  function Parser(){
    $this->contentArray = array();
  }

  function getContentArray(){
    return $this->contentArray;
  }
	
  /* Fügt Inhalt in das Inhalts-Array ein */
  function appendContent($newContent){
    if(is_array($newContent)){
      for($i = 0; $i < count($newContent); $i++){
        array_push($this->contentArray, $newContent[$i]);
      }
    } else {
      array_push($this->contentArray, $newContent);
    }
  }

  /* Zeigt den Geparsten Inhalt an */
  function printContent(){
    $array = $this->getContentArray();
    for ($i = 0; $i < count($array); $i++){
      echo $array[$i];
    }
  }


  function parseContent($fileName, & $callingObject, $filePart=null){
    $fileArray = file($fileName);													/* File als Array einlesen */
    if($filePart != null){
      $fileArray = $this->_fetchFilePart(&$fileArray, $filePart);									/* Wenn File aus mehreren Template-Teilen besteht, dann wird hir der relevante Zeil geholt */
    }
    for($i = 0; $i < count($fileArray); $i++){												/* Das Array durchlaufen ... */
      if(0 != preg_match_all("/\{content:([a-z]+):([a-z0-9_]+)\}/i", $fileArray[$i], $results)){
        //print_r($results);
        for($j = 0; $j < count($results[1]); $j++){
          $insert = $callingObject->$results[1][$j]($results[2][$j]);
	  $fileArray[$i] = preg_replace("/\{content:".$results[1][$j].":".$results[2][$j]."\}/i", $insert, $fileArray[$i]);
        }
      }
    }
    $this->appendContent($fileArray);
  }


  /* Sucht innerhalb eines Template-Files nach dem richtigem Template-stück 
   * und pappt es in ein neues Array */
  function _fetchFilePart($fileArray, $filePart){
    $inPart = false;									/* Flag ob innerhalb des gesuchten Templates Initialisieren */
    $newArray = array();								/* Neues File-Array */
    for($i = 0; $i < count($fileArray); $i++){						/* Altes Array dtrchlaufen */
      if($inPart){
        if(preg_match("/\{content:part:end\}/i", $fileArray[$i])){			/* Wenn im gesuchtem Template, dann nach {content:part:end\} suchen */
          $inPart = false;								/* ...wenn gefunden Flag wieder False setzen */
          break;									/* ...und Schleife abbrechen */
        } else {
          array_push($newArray, $fileArray[$i]);					/* An sonsten Zeile zum neuem Array hinzufügen */
	}
      } else {										/* Wenn nich im gesuchtem Template */
        if(preg_match("/\{content:part:".$filePart."\}/i", $fileArray[$i])){		/* Nach dem Anfang des Templates suchen */
          $inPart = true;								/* und wenn gefunden, dann Flaf true setzen */
	}
      }
    }
    return $newArray;									/* Neues Array zurueckgeben */
  }

}
?>
