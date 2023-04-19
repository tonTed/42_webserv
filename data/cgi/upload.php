<?php

$upload_dir = 'data/files';

if ($_FILES['file']['error'] === UPLOAD_ERR_OK) {
    $tmp_name = $_FILES['file']['tmp_name'];
    $name = basename($_FILES['file']['name']);

    if (move_uploaded_file($tmp_name, "$upload_dir/$name")) {
        echo "<h1>Le fichier $name a été téléchargé avec succès.</h1>";
    } else {
        echo "<h1>Erreur : impossible de télécharger le fichier.</h1>";
    }
} else {
    echo "<h1>Erreur : le fichier n'a pas été envoyé.</h1>";
}

?>
