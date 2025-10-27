document.addEventListener('DOMContentLoaded', () => {
    const deleteBtn = document.getElementById('deleteButton');
    const statusDiv = document.getElementById('statusMessageDelete');
    const deleteFileInput = document.getElementById('deleteFileInput'); 

    deleteBtn.addEventListener('click', () => {
        
        const filePathToDelete = deleteFileInput.value; 
        if (!filePathToDelete) {
            statusDiv.style.color = 'red';
            statusDiv.textContent = 'Veuillez entrer un chemin de fichier à supprimer.';
            return;
        }

  
        if (!confirm(`Êtes-vous sûr de vouloir supprimer le fichier ${filePathToDelete} ?`)) {
            return; 
        }


        fetch(filePathToDelete, {
            method: 'DELETE'
        })
        .then(response => {
            if (response.ok) {
                console.log('Fichier supprimé avec succès.');
                statusDiv.style.color = 'green';
                statusDiv.textContent = `Le fichier ${filePathToDelete} a été supprimé avec succès ! (Statut: ${response.status})`;
            } else {
                console.error('Échec de la suppression.');
                statusDiv.style.color = 'red';
                statusDiv.textContent = `Erreur lors de la suppression de ${filePathToDelete}. Le serveur a répondu avec le statut : ${response.status}`;
            }
        })
        .catch(error => {
            console.error('Erreur réseau:', error);
            statusDiv.style.color = 'red';
            statusDiv.textContent = 'Erreur réseau. Impossible de contacter le serveur.';
        });
    });
});

document.addEventListener('DOMContentLoaded', () => {

    const uploadForm = document.getElementById('uploadForm');
    const fileInput = document.getElementById('monFichier');
    const statusMessage = document.getElementById('statusMessagePost');

    uploadForm.addEventListener('submit', (event) => {
        event.preventDefault();

        const file = fileInput.files[0];

        if (!file) {
            statusMessage.textContent = 'Veuillez sélectionner un fichier à envoyer.';
            statusMessage.style.color = 'red';
            return;
        }

        const formData = new FormData();
        formData.append('monFichier', file);

        statusMessage.textContent = 'Téléversement en cours...';
        statusMessage.style.color = 'black';

        fetch('/cgi/bin/upload.php', {
            method: 'POST',
            body: formData
        })
        .then(response => {
            if (!response.ok) {
                throw new Error(`Erreur serveur : ${response.statusText}`);
            }
            return response.json();
        })
        .then(data => {
            if (data.success) {
                statusMessage.textContent = data.message;
                statusMessage.style.color = 'green';
            } else {
                statusMessage.textContent = 'Erreur : ' + data.message;
                statusMessage.style.color = 'red';
            }
        })
        .catch(error => {
            console.error('Erreur lors de l\'envoi du fichier:', error);
            statusMessage.textContent = 'Une erreur est survenue lors de la communication avec le serveur.';
            statusMessage.style.color = 'red';
        });
    });
});