<?php
// Active l'affichage des erreurs pour le débogage
ini_set('display_errors', 1);
error_reporting(E_ALL);

// Définit le dossier de destination pour les uploads
$dossierDestination = "uploads/";

// Vérifie si la méthode de la requête est bien POST
if ($_SERVER["REQUEST_METHOD"] == "POST") {

    // Vérifie si un fichier a été uploadé et s'il n'y a pas d'erreur
    if (isset($_FILES["monFichier"]) && $_FILES["monFichier"]["error"] == 0) {
        
        $nomFichierOriginal = basename($_FILES["monFichier"]["name"]);
        $cheminFichierCible = $dossierDestination . $nomFichierOriginal;
        
        // --- Section de Sécurité ---
        
        // 1. Vérifier la taille du fichier (ex: max 5MB)
        $tailleMax = 5 * 1024 * 1024; // 5 Megabytes
        if ($_FILES["monFichier"]["size"] > $tailleMax) {
            die("Erreur : Le fichier est trop volumineux. La taille maximale est de 5MB.");
        }
        
        // 2. Filtrer les types de fichiers autorisés (liste blanche)
        $typesAutorises = [
            'jpg' => 'image/jpeg',
            'png' => 'image/png',
            'gif' => 'image/gif',
            'pdf' => 'application/pdf'
        ];
        $extensionFichier = strtolower(pathinfo($cheminFichierCible, PATHINFO_EXTENSION));
        if (!array_key_exists($extensionFichier, $typesAutorises)) {
            die("Erreur : Type de fichier non autorisé.");
        }
        
        // 3. (Optionnel mais recommandé) Sécuriser le nom du fichier
        // Supprime les caractères spéciaux et remplace les espaces par des underscores
        $nomFichierSecurise = preg_replace("/[^A-Za-z0-9._-]/", "_", $nomFichierOriginal);
        // Ajoute un identifiant unique pour éviter les écrasements de fichiers
        $cheminFichierFinal = $dossierDestination . uniqid() . '-' . $nomFichierSecurise;

        // --- Fin de la Section Sécurité ---

        // Déplace le fichier du dossier temporaire vers la destination finale
        // La fonction move_uploaded_file() vérifie que le fichier a bien été uploadé via POST
        if (move_uploaded_file($_FILES["monFichier"]["tmp_name"], $cheminFichierFinal)) {
            echo "Le fichier ". htmlspecialchars($nomFichierOriginal). " a été envoyé avec succès.";
        } else {
            echo "Désolé, une erreur est survenue lors de l'envoi du fichier.";
        }

    } else {
        echo "Erreur : Aucun fichier n'a été envoyé ou une erreur s'est produite.";
    }
} else {
    // Si la méthode n'est pas POST, on refuse la requête
    header("HTTP/1.0 405 Method Not Allowed");
    echo "Méthode non autorisée.";
}
?>