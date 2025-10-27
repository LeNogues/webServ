#!/usr/bin/php-cgi
<?php

header("Content-Type: application/json");

$response = ['success' => false, 'message' => 'Une erreur inconnue est survenue.'];

if (isset($_FILES['monFichier'])) {
    $uploadDir = 'uploads/';
    $uploadFile = $uploadDir . basename($_FILES['monFichier']['name']);

    if (!is_dir($uploadDir)) {
        mkdir($uploadDir, 0755, true);
    }

    if ($_FILES['monFichier']['error'] === UPLOAD_ERR_OK) {
        if (move_uploaded_file($_FILES['monFichier']['tmp_name'], $uploadFile)) {
            $response['success'] = true;
            $response['message'] = "Le fichier " . htmlspecialchars(basename($_FILES['monFichier']['name'])) . " a été téléversé.";
        } else {
            $response['message'] = "Échec du déplacement du fichier téléversé.";
        }
    } else {

        switch ($_FILES['monFichier']['error']) {
            default:
                $response['message'] = "Erreur de téléversement inconnue.";
                break;
        }
    }
} else {
    $response['message'] = "Aucun fichier reçu sous la clé 'monFichier'. Assurez-vous que le formulaire envoie bien le fichier avec ce nom.";
}

echo json_encode($response);
?>

  