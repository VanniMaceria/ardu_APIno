import 'dart:convert';
import 'package:dart_frog/dart_frog.dart';
import 'package:mqtt_client/mqtt_client.dart';
import 'package:mqtt_client/mqtt_server_client.dart';
import 'package:typed_data/typed_data.dart';

final mqttClient = MqttServerClient('test.mosquitto.org', '1883');
int voltage = 0;
String color = '';

/// @Allow(POST)
Future<Response> onRequest(RequestContext context) async {
  final request = context.request;
  if (request.method == HttpMethod.post) {
    return doPost(request);
  }
  return Response(body: 'HTTP method not valid');
}

Future<Response> doPost(Request request) async {
  final body = await request.body();
  final data = jsonDecode(body) as Map<String, dynamic>;

  // Estrarre 'voltage' dalla richiesta e verificare che sia un intero
  if (data.containsKey('voltage') &&
      data['voltage'] is int &&
      data.containsKey('color') &&
      data['color'] is String) {
    voltage = data['voltage'] as int;
    color = data['color'] as String;
  } else {
    return Response.json(
      statusCode: 400,
      body: {'error': 'Invalid voltage value'},
    );
  }

  mqttClient
    ..logging(on: true)
    ..keepAlivePeriod = 60;

  // ignore: cascade_invocations
  mqttClient.onConnected = onConnected;
  mqttClient.onDisconnected = onDisconnected;

  await connectToMQTTBroker();

  return Response.json(
      body: {'status': 'Voltage and Color successfully forwarded'});
}

Future<void> connectToMQTTBroker() async {
  try {
    await mqttClient.connect();
  } catch (e) {
    print('Errore di connessione: $e');
    mqttClient.disconnect();
    return;
  }

  if (mqttClient.connectionStatus!.state == MqttConnectionState.connected) {
    print('Connesso al broker MQTT');
    await publishAtTopic('voltaggio_led_mia_api', voltage, color);
  } else {
    print(
      'Connessione fallita con codice ${mqttClient.connectionStatus!.state}',
    );
    mqttClient.disconnect();
  }
}

Future<void> publishAtTopic(String topic, int voltage, String color) async {
  // Creazione dell'oggetto JSON
  Map<String, dynamic> data = {'voltage': voltage, 'color': color};

  // Conversione in stringa JSON
  String jsonString = jsonEncode(data);

  // Conversione della stringa JSON in Uint8Buffer
  Uint8Buffer buffer = Uint8Buffer()..addAll(utf8.encode(jsonString));

  // Pubblica il messaggio su MQTT
  mqttClient.publishMessage(topic, MqttQos.atLeastOnce, buffer);
}

void onConnected() {
  print('Client connesso al broker MQTT');
}

void onDisconnected() {
  print('Client disconnesso dal broker MQTT');
}
