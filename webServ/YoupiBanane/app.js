// app.js
document.addEventListener('DOMContentLoaded', () => {
    // === DELETE HANDLER ===
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

        if (!confirm(`Êtes-vous sûr de vouloir supprimer le fichier ${filePathToDelete} ?`)) return;

        fetch(filePathToDelete, { method: 'DELETE' })
            .then(response => {
                if (response.ok) {
                    statusDiv.style.color = 'green';
                    statusDiv.textContent = `Le fichier ${filePathToDelete} a été supprimé avec succès ! (Statut: ${response.status})`;
                } else {
                    statusDiv.style.color = 'red';
                    statusDiv.textContent = `Erreur lors de la suppression de ${filePathToDelete}. Statut : ${response.status}`;
                }
            })
            .catch(() => {
                statusDiv.style.color = 'red';
                statusDiv.textContent = 'Erreur réseau. Impossible de contacter le serveur.';
            });
    });

    // === GET HANDLER ===
    const getBtn = document.getElementById('getButton');
    const getFileInput = document.getElementById('getFileInput');
    const getStatusDiv = document.getElementById('getStatusMessage');
    const getResultFrame = document.getElementById('getResultFrame');

    getBtn.addEventListener('click', () => {
        const filePathToGet = getFileInput.value;
        if (!filePathToGet) {
            getStatusDiv.style.color = 'red';
            getStatusDiv.textContent = 'Veuillez entrer un chemin de fichier à récupérer.';
            return;
        }

        getStatusDiv.style.color = 'black';
        getStatusDiv.textContent = `Récupération de ${filePathToGet}...`;

        fetch(filePathToGet)
            .then(response => {
                if (response.ok) {
                    getStatusDiv.style.color = 'green';
                    getStatusDiv.textContent = `Fichier ${filePathToGet} récupéré avec succès.`;
                    // Display the result in iframe
                    getResultFrame.src = filePathToGet;
                } else {
                    throw new Error(`Erreur HTTP ${response.status}`);
                }
            })
            .catch(error => {
                getStatusDiv.style.color = 'red';
                getStatusDiv.textContent = `Échec de la récupération : ${error.message}`;
                getResultFrame.srcdoc = `<p style="color:red;">Erreur : ${error.message}</p>`;
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

        fetch('/content/img/', {
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
