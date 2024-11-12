// lib/get_weather.dart
import 'package:flutter/material.dart';
import 'weather_service.dart';
import 'package:intl/intl.dart';

class WeatherScreen extends StatefulWidget {
  @override
  _WeatherScreenState createState() => _WeatherScreenState();
}

class _WeatherScreenState extends State<WeatherScreen> {
  final WeatherService _weatherService = WeatherService();
  Map<String, dynamic>? weatherData;

  @override
  void initState() {
    super.initState();
    _fetchData();
  }

  Future<void> _fetchData() async {
    try {
      final data = await _weatherService.fetchWeatherData();
      setState(() {
        weatherData = data;
      });
    } catch (e) {
      print(e);
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Weather Data'),
        centerTitle: true,
        backgroundColor: Colors.teal,
      ),
      body: weatherData == null
          ? Center(child: CircularProgressIndicator())
          : Padding(
              padding: const EdgeInsets.all(16.0),
              child: ListView(
                children: [
                  _buildSectionTitle('Overview'),
                  _buildOverviewCard(),
                  SizedBox(height: 20),
                  _buildSectionTitle('Temperature and Humidity Details'),
                  _buildDetailsList(),
                  SizedBox(height: 20),
                  _buildSectionTitle('Monthly Max Temperature and Humidity'),
                  _buildMonthYearVerticalList(weatherData!["month_year_max"]),
                ],
              ),
            ),
    );
  }

  Widget _buildSectionTitle(String title) {
    return Text(
      title,
      style: TextStyle(
        fontSize: 18,
        fontWeight: FontWeight.bold,
        color: Colors.teal,
      ),
    );
  }

  Widget _buildOverviewCard() {
    return Card(
      elevation: 4,
      shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(10)),
      child: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            Text('Maximum Temperature: ${weatherData!["suhumax"]} °C',
                style: TextStyle(fontSize: 16)),
            Text('Minimum Temperature: ${weatherData!["suhumin"]} °C',
                style: TextStyle(fontSize: 16)),
            Text('Average Temperature: ${weatherData!["suhurata"]} °C',
                style: TextStyle(fontSize: 16)),
          ],
        ),
      ),
    );
  }

  Widget _buildDetailsList() {
    return Column(
      children: weatherData!["nilai_suhu_max_humid_max"].map<Widget>((data) {
        return Card(
          elevation: 2,
          shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(8)),
          child: ListTile(
            title: Text(
              'ID: ${data["id"]}',
              style: TextStyle(fontWeight: FontWeight.bold),
            ),
            subtitle: Column(
              crossAxisAlignment: CrossAxisAlignment.start,
              children: [
                Text('Temperature: ${data["suhu"]} °C'),
                Text('Humidity: ${data["humid"]}%'),
                Text('Lux: ${data["lux"]}'),
                Text('Date: ${data["ts"]}'),
              ],
            ),
          ),
        );
      }).toList(),
    );
  }

  String formatMonthYear(String monthYear) {
    try {
      final date = DateFormat("MM-yy").parse(monthYear);
      String monthYearLong = DateFormat("MMMM - yyyy").format(date);
      return "$monthYearLong / $monthYear";
    } catch (e) {
      print("Error formatting date: $e");
      return monthYear;
    }
  }

  Widget _buildMonthYearVerticalList(List<dynamic> monthYearList) {
    return Row(
      children: [
        // Kolom vertikal di kiri untuk data bulan dan tahun
        Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: monthYearList.map((monthYear) {
            return Padding(
              padding: const EdgeInsets.symmetric(vertical: 4.0),
              child: Card(
                elevation: 2,
                shape: RoundedRectangleBorder(
                  borderRadius: BorderRadius.circular(8),
                ),
                child: Padding(
                  padding: const EdgeInsets.all(12.0),
                  child: Text(
                    formatMonthYear(monthYear["bulan"]),
                    style: TextStyle(fontSize: 14, color: Colors.black87),
                    textAlign: TextAlign.left,
                  ),
                ),
              ),
            );
          }).toList(),
        ),
      ],
    );
  }
}
