const express = require('express');
const path = require('path');
const { execFile } = require('child_process');
const app = express();
const port = process.env.PORT || 3000;

app.use(express.static(path.join(__dirname, 'public')));

app.get('/ping', (req, res) => res.json({ok:true, message:'sito operativo'}));

// API che richiama l'helper C per ottenere l'inventario in formato JSON
app.get('/api/inventario', (req, res) => {
	const exe = process.platform === 'win32' ? 'sart_web.exe' : './sart_web';
	// Lancio il processo nella cartella superiore (repo root) così legge i file
	execFile(exe, { cwd: path.join(__dirname, '..') }, (err, stdout, stderr) => {
		if(err){
			// Se non trovato l'eseguibile prova a eseguire senza percorso
			console.error('exec error', err, stderr);
			return res.status(500).json({ error: stderr || String(err) });
		}
		try{
			const obj = JSON.parse(stdout);
			return res.json(obj.inventario ? { inventario: obj.inventario } : obj);
		}catch(e){
			return res.status(200).type('json').send(stdout);
		}
	});
});

app.get('/api/progetti', (req, res) => {
	const exe = process.platform === 'win32' ? 'sart_web.exe' : './sart_web';
	execFile(exe, { cwd: path.join(__dirname, '..') }, (err, stdout, stderr) => {
		if(err){
			console.error('exec error', err, stderr);
			return res.status(500).json({ error: stderr || String(err) });
		}
		try{
			const obj = JSON.parse(stdout);
			return res.json(obj.progetti ? { progetti: obj.progetti } : obj);
		}catch(e){
			return res.status(200).type('json').send(stdout);
		}
	});
});

app.listen(port, () => console.log(`Server statico avviato su http://localhost:${port}`));
