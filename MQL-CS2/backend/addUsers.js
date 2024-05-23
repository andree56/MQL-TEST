const mongoose = require('mongoose');
const UID = require('./models/UID');

// Connect to MongoDB
mongoose.connect('mongodb://localhost:27017/rfid', { useNewUrlParser: true, useUnifiedTopology: true })
    .then(() => {
        console.log("Connected to MongoDB");
    })
    .catch(err => {
        console.error("Error connecting to MongoDB", err);
    });

// List of users to add
const users = [
    { uid: ["0xb6", "0x99", "0x8b", "0x2b"], name: 'User 1' },
    { uid: ["0xaf", "0x33", "0x4d", "0x1c"], name: 'User 2' }
];

(async () => {
    try {
        await UID.deleteMany({}); // Clear existing users
        console.log("Existing users cleared");
        await UID.insertMany(users); // Insert new users
        console.log('Users added');
    } catch (err) {
        console.error("Error adding users", err);
    } finally {
        mongoose.connection.close();
    }
})();
