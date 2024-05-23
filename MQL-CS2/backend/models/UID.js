const mongoose = require('mongoose');

const uidSchema = new mongoose.Schema({
    uid: [String], // Store UID as an array of strings
    name: String,
    department: String // Add department field
});

module.exports = mongoose.model('UID', uidSchema);
