import 'dart:convert';
import 'package:http/http.dart' as http;

class WeatherService {
  final String apiUrl = 'http://192.168.18.5/PEM_IOT/UTS/backend.php';

  Future<Map<String, dynamic>> fetchWeatherData() async {
    final response = await http.get(Uri.parse(apiUrl));
    if (response.statusCode == 200) {
      return json.decode(response.body);
    } else {
      throw Exception('Failed to load weather data');
    }
  }
}
