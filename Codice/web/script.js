// Script per caricare e visualizzare l'inventario dei tessuti (Inventario.txt).
// Contesto: questo file si trova in `web/` e quando si serve la cartella `Codice`
// il file sorgente `Inventario.txt` è nella cartella padre (`../Inventario.txt`).

/**
 * loadInventory
 * - Legge il file `Inventario.txt` via fetch, lo suddivide in righe e ritorna
 *   un array di oggetti che rappresentano i singoli tessuti.
 * - Nota: la prima riga del file è metadata (es. numero di elementi) e viene scartata.
 * - Il parsing dei campi è "best-effort": il file sembra essere separato da
 *   spazi e contiene sia token testuali (nome, categoria, colore, ...) che
 *   numerici (quantità, larghezza, prezzi, data). Se il formato cambia,
 *   bisognerà aggiornare questa mappatura.
 */
async function loadInventory() {
  // Percorso relativo: dal file `web/script.js` si sale di una cartella
  // per leggere `Codice/Inventario.txt` quando il server è impostato su `Codice`.
  const resp = await fetch('../Inventario.txt');
  if (!resp.ok) throw new Error('Impossibile caricare Inventario.txt: ' + resp.status);
  const txt = await resp.text();

  // Divido il testo in righe, rimuovo eventuali righe vuote
  const lines = txt.trim().split(/\r?\n/).filter(Boolean);
  if (lines.length === 0) return [];

  // Scarto la prima riga (metadata) e considero ogni riga successiva come un elemento
  const dataLines = lines.slice(1);

  // Mappatura dei token: adattare se il formato cambia
  const items = dataLines.map(line => {
    const tokens = line.split(/\s+/);
    const name = tokens[0] || '';        // es. Sintetico_Giallo
    const category = tokens[1] || '';    // es. TessutiSintetici
    const material = tokens[2] || '';    // es. Sintetico
    const color = tokens[3] || '';       // es. Giallo
    const pattern = tokens[4] || '';     // es. Nessuna / Righe
    const quantity = parseFloat(tokens[5]) || 0; // quantità (es. metri)
    const width = parseFloat(tokens[6]) || 0;    // larghezza o altro valore numerico
    const code = tokens[7] || '';        // codice stringa (es. Yellow80)
    // Il prezzo è stato mappato qui come token[13] (best-effort). Se hai
    // un significato preciso per le colonne, comunicamelo e aggiorno il mapping.
    const price = parseFloat(tokens[13]) || 0;

    // La data sembra trovarsi in tre token separati (giorno mese anno)
    let date = '';
    if (tokens[10] && tokens[11] && tokens[12]) {
      // Uso padStart per garantire 2 cifre per giorno/mese
      date = `${tokens[10].padStart(2,'0')}/${tokens[11].padStart(2,'0')}/${tokens[12]}`;
    }

    return { name, category, material, color, pattern, quantity, width, code, price, date };
  });
  return items;
}


/**
 * renderInventory
 * - Riceve un array di oggetti e popola la tabella HTML (`#inventory tbody`).
 */
function renderInventory(items) {
  const tbody = document.querySelector('#inventory tbody');
  tbody.innerHTML = '';
  for (const it of items) {
    const tr = document.createElement('tr');
    // escapeHtml viene usata per prevenire injection di HTML
    tr.innerHTML = `
      <td>${escapeHtml(it.name)}</td>
      <td>${escapeHtml(it.category)}</td>
      <td>${escapeHtml(it.material)}</td>
      <td>${escapeHtml(it.color)}</td>
      <td>${escapeHtml(it.pattern)}</td>
      <td>${it.quantity}</td>
      <td>${it.width}</td>
      <td>${escapeHtml(it.code)}</td>
      <td>${it.price}</td>
      <td>${escapeHtml(it.date)}</td>
    `;
    tbody.appendChild(tr);
  }
}


/**
 * escapeHtml
 * - Semplice funzione di utilità per sostituire i caratteri speciali HTML
 *   con le rispettive entità (evita problemi se i campi contengono simboli).
 */
function escapeHtml(s) {
  return (s+'').replace(/[&<>"']/g, c => ({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":"&#39;"}[c]));
}


/**
 * setupSearch
 * - Collega l'input di ricerca (`#search`) per filtrare l'array di elementi
 *   usando confronto case-insensitive su `name`, `color`, `category`, `material`.
 */
function setupSearch(items) {
  const input = document.getElementById('search');
  input.addEventListener('input', () => {
    const q = input.value.trim().toLowerCase();
    if (!q) return renderInventory(items);
    const filtered = items.filter(it =>
      it.name.toLowerCase().includes(q) ||
      it.color.toLowerCase().includes(q) ||
      it.category.toLowerCase().includes(q) ||
      it.material.toLowerCase().includes(q)
    );
    renderInventory(filtered);
  });
}


// IIFE principale: carica dati, renderizza tabella e imposta la ricerca.
(async function main(){
  try {
    const items = await loadInventory();
    renderInventory(items);
    setupSearch(items);
  } catch (e) {
    // In caso di errore mostro un messaggio semplice nella pagina
    document.body.innerHTML = '<p class="error">Errore: ' + e.message + '</p>';
    console.error(e);
  }
})();
