 // Your web app's Firebase configuration
 const firebaseConfig = {
  apiKey: "AIzaSyA0HueF1NGp5rDgadDJmTwh4r7Bx1-opwM",
  authDomain: "broadcast-message-ac07f.firebaseapp.com",
  databaseURL: "https://broadcast-message-ac07f-default-rtdb.firebaseio.com",
  projectId: "broadcast-message-ac07f",
  storageBucket: "broadcast-message-ac07f.appspot.com",
  messagingSenderId: "362272250556",
  appId: "1:362272250556:web:dc4ce9e3c8e3ae22d40eb9"
};
  // Initialize Firebase
  firebase.initializeApp(firebaseConfig);
var counter = 1;
// Refernece contactInfo collections
let contactInfo = firebase.database().ref("LoRa");

// Listen for a submit
document.querySelector(".contact-form").addEventListener("submit", submitForm);

function submitForm(e) {
  e.preventDefault();

  //   Get input Values
 
  let message = document.querySelector(".message").value;
  console.log(message);

  saveContactInfo(message);

  document.querySelector(".contact-form").reset();
}

// Save infos to Firebase
function saveContactInfo(message) {
  var db = firebase.database();
db.ref("LoRa/message").set(message);
db.ref(String(counter)).set(message);
counter = counter +1;
}
