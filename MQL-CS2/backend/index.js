const express = require('express');
const bodyParser = require('body-parser');
const mongoose = require('mongoose');
const cors = require('cors'); // Import cors
const UID = require('./models/UID');
const Attendance = require('./models/Attendance');

// Connect to MongoDB
mongoose.connect('mongodb://localhost:27017/rfid', { useNewUrlParser: true, useUnifiedTopology: true });

const app = express();

// Use CORS middleware
app.use(cors({
    origin: 'http://localhost:3000' // Allow requests from your React app
}));

app.use(bodyParser.json());

app.get('/attendance/total', async (req, res) => {
    const total = await Attendance.countDocuments({});
    res.json({ total });
});

app.get('/attendance/by-department', async (req, res) => {
    try {
    const data = await Attendance.aggregate([
        { $group: { _id: '$department', count: { $sum: 1 } } }
    ]);
    res.json(data);
    } catch (error) {
    res.status(500).json({ error: 'Failed to fetch attendance by department' });
    }
});

app.get('/attendance/top-attendee', async (req, res) => {
    try {
    const data = await Attendance.aggregate([
        { $group: { _id: '$name', count: { $sum: 1 } } },
        { $sort: { count: -1 } },
        { $limit: 1 }
    ]);
    res.json(data[0]);
    } catch (error) {
    res.status(500).json({ error: 'Failed to fetch top attendee' });
    }
});

app.get('/attendance', async (req, res) => {
    try {
        const attendances = await Attendance.find();
        res.json(attendances);
    } catch (err) {
        res.status(500).json({ success: false, error: err.message });
    }
});

// Endpoint to check if UID is whitelisted
app.post('/checkUID', async (req, res) => {
    const { uid } = req.body;
    const user = await UID.findOne({ uid });
    if (user) {
        res.json({ success: true, name: user.name, department: user.department });
    } else {
        res.json({ success: false });
    }
});

// app.post('/checkUID', async (req, res) => {
//     const { uid } = req.body;
//     const user = await UID.findOne({ uid });
//     if (user) {
//         res.json({ success: true, name: user.name});
//     } else {
//         res.json({ success: false });
//     }
// });

// Endpoint to log attendance
app.post('/logAttendance', async (req, res) => {
    const { uid, name, department } = req.body;
    const attendance = new Attendance({ uid, name, department });
    await attendance.save();
    res.json({ success: true });
});



const port = 3001;
app.listen(port, () => {
    console.log(`Server running on port ${port}`);
});
