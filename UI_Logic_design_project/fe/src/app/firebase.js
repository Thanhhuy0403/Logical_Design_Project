// Import the functions you need from the SDKs you need
import { initializeApp } from "firebase/app";
// TODO: Add SDKs for Firebase products that you want to use
// https://firebase.google.com/docs/web/setup#available-libraries
import { getDatabase } from "firebase/database";

// Your web app's Firebase configuration
const firebaseConfig = {
    apiKey: process.env.API_KEY,
    authDomain: "app-api-3b3ff.firebaseapp.com",
    projectId: "app-api-3b3ff",
    storageBucket: "app-api-3b3ff.firebasestorage.app",
    messagingSenderId: "17759580476",
    appId: "1:17759580476:web:96393e99948cd0bd432e39",
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);
const database = getDatabase(app);
export default database;
