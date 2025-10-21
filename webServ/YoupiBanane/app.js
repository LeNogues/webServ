document.addEventListener('DOMContentLoaded', () => {
    const deleteBtn = document.getElementById('deleteButton');
    const statusDiv = document.getElementById('statusMessage');
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