const DATA_URL = "../data/processed/qsjh2h_example.json";
const SUITS = { s: "♠", h: "♥", d: "♦", c: "♣" };
const stats = { total: 0, good: 0, ok: 0, mistake: 0 };
let data;
let current;

const $ = (id) => document.getElementById(id);

function cardsHTML(str) {
  return str.match(/.{2}/g).map((c) => {
    const red = c[1] === "h" || c[1] === "d";
    return `<span class="card ${red ? "red" : ""}">${c[0]}${SUITS[c[1]]}</span>`;
  }).join("");
}

function pick(arr) {
  return arr[Math.floor(Math.random() * arr.length)];
}

// Les joueurs alternent à chaque action dans l'arbre du flop
function describePath(path) {
  if (path === "root") return "Aucune action avant toi.";
  const first = data.nodes.root.position;
  const second = first === "OOP" ? "IP" : "OOP";
  return path.split("/").map((a, i) => `${i % 2 === 0 ? first : second} : ${a}`).join(" → ");
}

// Barème de la v1 (convention du projet, pas une règle du solveur)
function grade(freq) {
  if (freq >= 0.30) return ["good", "Bonne réponse"];
  if (freq >= 0.05) return ["ok", "Acceptable (action mixte)"];
  return ["mistake", "Erreur"];
}

function newSpot() {
  const path = pick(Object.keys(data.nodes));
  const node = data.nodes[path];
  const hand = pick(Object.keys(node.hands));
  current = { path, node, hand };

  $("board").innerHTML = cardsHTML(data.board);
  $("context").textContent =
    `Pot de départ : ${data.pot}, stack effectif : ${data.stack}. Tu es ${node.position}. ${describePath(path)}`;
  $("hand").innerHTML = cardsHTML(hand);
  $("result").innerHTML = "";
  $("next").hidden = true;

  $("actions").innerHTML = "";
  node.actions.forEach((action, i) => {
    const b = document.createElement("button");
    b.textContent = action;
    b.addEventListener("click", () => answer(i));
    $("actions").appendChild(b);
  });
}

function answer(i) {
  const { node, hand } = current;
  const freqs = node.hands[hand];
  const [cls, label] = grade(freqs[i]);

  stats.total++;
  stats[cls]++;

  $("actions").querySelectorAll("button").forEach((b) => (b.disabled = true));
  const lines = node.actions.map((a, j) =>
    `<div class="${j === i ? "chosen" : ""}">${a} : ${(freqs[j] * 100).toFixed(1)} %${j === i ? " ← ton choix" : ""}</div>`
  ).join("");
  $("result").innerHTML = `<p class="${cls}">${label}</p>${lines}`;
  $("next").hidden = false;
  $("stats").textContent =
    `Score : ${stats.good} bonnes, ${stats.ok} acceptables, ${stats.mistake} erreurs sur ${stats.total}`;
}

$("next").addEventListener("click", newSpot);

fetch(DATA_URL)
  .then((r) => r.json())
  .then((d) => { data = d; newSpot(); })
  .catch((e) => { $("context").textContent = `Erreur de chargement : ${e}`; });