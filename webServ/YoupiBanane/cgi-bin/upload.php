    
#!/usr/bin/php-cgi
<?php

header("Content-Type: application/json");

$response = ['success' => false, 'message' => 'Une erreur inconnue est survenue.'];

// CORRECTION : Utiliser 'monFichier' au lieu de 'fileToUpload'
if (isset($_FILES['monFichier'])) {
    $uploadDir = 'uploads/'; // Assurez-vous que ce dossier existe et est accessible en écriture
    // CORRECTION : Utiliser 'monFichier'
    $uploadFile = $uploadDir . basename($_FILES['monFichier']['name']);

    if (!is_dir($uploadDir)) {
        mkdir($uploadDir, 0755, true);
    }

    // CORRECTION : Utiliser 'monFichier'
    if ($_FILES['monFichier']['error'] === UPLOAD_ERR_OK) {
        // Déplace le fichier temporaire vers son emplacement final
        // CORRECTION : Utiliser 'monFichier'
        if (move_uploaded_file($_FILES['monFichier']['tmp_name'], $uploadFile)) {
            $response['success'] = true;
            // CORRECTION : Utiliser 'monFichier'
            $response['message'] = "Le fichier " . htmlspecialchars(basename($_FILES['monFichier']['name'])) . " a été téléversé.";
        } else {
            $response['message'] = "Échec du déplacement du fichier téléversé.";
        }
    } else {
        // Gère les différentes erreurs de téléversement
        // CORRECTION : Utiliser 'monFichier'
        switch ($_FILES['monFichier']['error']) {
            // ... tous les cas d'erreur restent les mêmes
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

  