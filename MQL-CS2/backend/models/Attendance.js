const mongoose = require('mongoose');

const attendanceSchema = new mongoose.Schema({
    uid: [String], // Store UID as an array of strings
    name: String,
    department: String, // Add department field
    timestamp: {
        type: String,
        default: function() {
        return new Date().toLocaleString('en-US', { timeZone: 'Asia/Manila' });// Adjust the timeZone to your local time
        }
    }
});

module.exports = mongoose.model('Attendance', attendanceSchema);
