<?

  /* copyright: Jan Losinski, 2006
     
     at the Moment this is no free software, look at the
     COPYING-File in the Main-Directory for License-Terms
  */

class Parser{	
  var $contentArray;

  function Parser(){
    $this->contentArray = array();
  }

  function getContentArray(){
    return $this->contentArray;
  }
	
  /* Fuegt Inhalt in das Inhalts-Array ein */
  function appendContent($newContent){
    if(is_array($newContent)){
      $newContentCount = count($newContent);
      for($i = 0; $i < $newContentCount; $i++){
        array_push($this->contentArray, $newContent[$i]);
	//echo $newContent[$i]."\n";
      }
    } else {
      array_push($this->contentArray, $newContent);
      //echo $newContent."\n";
    }
  }

  /* Zeigt den Geparsten Inhalt an */
  function printContent(){
    $array = $this->getContentArray();
    $arrayCount = count($array);
    for ($i = 0; $i < $arrayCount; $i++){
      echo $array[$i];
    }
  }


  /* File Parsen */
  function parseContent($fileName, & $callingObject, $filePart=null, $lineWise = false){
    $fileArray = file($fileName);													/* File als Array einlesen */
    if($filePart != null){
      $fileArray = $this->_fetchFilePart(&$fileArray, $filePart);									/* Wenn File aus mehreren Template-Teilen besteht, dann wird hir der relevante Zeil geholt */
    }
    for($i = 0; $i < count($fileArray); $i++){												/* Das Array durchlaufen ... */
      if(0 != preg_match_all("/\{content:([a-z]+):([a-z0-9_]+)\}/i", $fileArray[$i], $results)){
        //print_r($results);
	$resultsCount = count($results[1]);
        for($j = 0; $j < $resultsCount; $j++){
          $insert = $callingObject->$results[1][$j]($results[2][$j]);
	  $fileArray[$i] = preg_replace("/\{content:".$results[1][$j].":".$results[2][$j]."\}/i", $insert, $fileArray[$i]);
        }
      }
      if ($lineWise){
        $this->appendContent($fileArray[$i]);
      }
    }
    if (!$lineWise){
      $this->appendContent($fileArray);
    }
  }


  /* Sucht innerhalb eines Template-Files nach dem richtigem Template-stueck 
   * und pappt es in ein neues Array */
  function _fetchFilePart($fileArray, $filePart){
    $inPart = false;									/* Flag ob innerhalb des gesuchten Templates Initialisieren */
    $newArray = array();								/* Neues File-Array */
    $fileArrayCount = count($fileArray);
    for($i = 0; $i < $fileArrayCount; $i++){						/* Altes Array dtrchlaufen */
      if($inPart){
        if(preg_match("/\{content:part:end\}/i", $fileArray[$i])){			/* Wenn im gesuchtem Template, dann nach {content:part:end\} suchen */
          $inPart = false;								/* ...wenn gefunden Flag wieder False setzen */
          break;									/* ...und Schleife abbrechen */
        } else {
          array_push($newArray, $fileArray[$i]);					/* An sonsten Zeile zum neuem Array hinzufuegen */
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
