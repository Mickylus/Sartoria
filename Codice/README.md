# Sartoria — Web UI locale

Questa cartella contiene una semplice interfaccia web che legge `Inventario.txt` e mostra l'elenco dei tessuti.

Come avviare (opzioni):

- Metodo rapido (Windows): esegui `serve.bat` nella cartella `Codice` e apri nel browser:

```
http://localhost:8000/web/index.html
```

- Alternativa: apri la cartella `Codice` in VS Code e usa l'estensione *Live Server*.

Note:
- Lo script JavaScript fa un "best-effort" per mappare i campi del file `Inventario.txt` — se il formato cambia, potrebbe essere necessario aggiornare `web/script.js`.
- In futuro posso aggiungere visualizzazione di `Progetti.txt` e funzionalità di editing.