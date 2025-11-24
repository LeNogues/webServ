// app.js
document.addEventListener('DOMContentLoaded', () => {
	// Helpers
	const setStatus = (el, message, color) => {
		el.style.color = color;
		el.textContent = message;
	};

	// DELETE HANDLER
	const btnDelete = document.getElementById('btnDelete');
	const inputDeletePath = document.getElementById('inputDeletePath');
	const statusDelete = document.getElementById('statusDelete');

	btnDelete.addEventListener('click', () => {
		const filePathToDelete = inputDeletePath.value;
		if (!filePathToDelete) {
			setStatus(statusDelete, 'Veuillez entrer un chemin de fichier à supprimer.', 'red');
			return;
		}

		if (!confirm(`Êtes-vous sûr de vouloir supprimer le fichier ${filePathToDelete} ?`)) return;

		fetch(filePathToDelete, { method: 'DELETE' })
			.then(response => {
				if (response.ok) {
					setStatus(statusDelete, `Le fichier ${filePathToDelete} a été supprimé avec succès ! (Statut: ${response.status})`, 'green');
				} else {
					setStatus(statusDelete, `Erreur lors de la suppression de ${filePathToDelete}. Statut : ${response.status}`, 'red');
				}
			})
			.catch(() => {
				setStatus(statusDelete, 'Erreur réseau. Impossible de contacter le serveur.', 'red');
			});
	});

	// GET HANDLER
	const btnGet = document.getElementById('btnGet');
	const inputGetPath = document.getElementById('inputGetPath');
	const statusGet = document.getElementById('statusGet');
	const iframeGetResult = document.getElementById('iframeGetResult');

	btnGet.addEventListener('click', () => {
		const filePathToGet = inputGetPath.value;
		if (!filePathToGet) {
			setStatus(statusGet, 'Veuillez entrer un chemin de fichier à récupérer.', 'red');
			return;
		}

		setStatus(statusGet, `Récupération de ${filePathToGet}...`, 'black');

		fetch(filePathToGet)
		.then(response => {
			if (response.ok) {
				setStatus(statusGet, `Fichier ${filePathToGet} récupéré avec succès.`, 'green');
				iframeGetResult.removeAttribute('srcdoc');
				iframeGetResult.src = filePathToGet;
			} else {
				throw new Error(`Erreur HTTP ${response.status}`);
			}
		})
		.catch(error => {
			setStatus(statusGet, `Échec de la récupération : ${error.message}`, 'red');
			iframeGetResult.src = 'about:blank';
			iframeGetResult.srcdoc = `<p style="color:red;">Erreur : ${error.message}</p>`;
		});
	});

	// UPLOAD HANDLER
	const formUpload = document.getElementById('formUpload');
	const inputUploadFile = document.getElementById('inputUploadFile');
	const statusUpload = document.getElementById('statusUpload');

	formUpload.addEventListener('submit', (event) => {
		event.preventDefault();

		const file = inputUploadFile.files[0];
		if (!file) {
			setStatus(statusUpload, 'Veuillez sélectionner un fichier à envoyer.', 'red');
			return;
		}

		const formData = new FormData();
		formData.append('monFichier', file);

		setStatus(statusUpload, 'Téléversement en cours...', 'black');

		fetch('/post_folder/', {
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
				setStatus(statusUpload, data.message, 'green');
			} else {
				setStatus(statusUpload, 'Erreur : ' + data.message, 'red');
			}
		})
		.catch(error => {
			console.error('Erreur lors de l\'envoi du fichier:', error);
			setStatus(statusUpload, 'Une erreur est survenue lors de la communication avec le serveur.', 'red');
		});
	});
});
