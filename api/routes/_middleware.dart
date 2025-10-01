import 'package:dart_frog/dart_frog.dart';

/// Middleware globale per gestire CORS in tutte le route.
///
/// - Intercetta tutte le richieste HTTP prima che arrivino alla route.
/// - Gestisce le richieste OPTIONS (preflight) restituendo status 204 e header CORS.
/// - Per le richieste normali, aggiunge gli header CORS alla risposta generata dalla route.
/// - Permette al frontend (es. Swagger UI) di fare richieste cross-origin senza errori.

Handler middleware(Handler handler) {
  return (context) async {
    // Preflight OPTIONS
    if (context.request.method == HttpMethod.options) {
      return Response(
        statusCode: 204,
        headers: {
          'Access-Control-Allow-Origin': '*',
          'Access-Control-Allow-Methods': 'GET, POST, PUT, DELETE, OPTIONS',
          'Access-Control-Allow-Headers': 'Content-Type, Authorization',
        },
      );
    }

    final response = await handler(context);

    // Otteniamo il body come String
    String? bodyString;

    if (response.body is String) {
      bodyString = response.body as String;
    } else if (response.body is Function) {
      // Closure () => Future<String> (come Response.json)
      bodyString = await (response.body as Future<String> Function())();
    } else {
      bodyString = response.body.toString();
    }

    return Response(
      statusCode: response.statusCode,
      body: bodyString,
      headers: {
        ...response.headers,
        'Access-Control-Allow-Origin': '*',
        'Access-Control-Allow-Methods': 'GET, POST, PUT, DELETE, OPTIONS',
        'Access-Control-Allow-Headers': 'Content-Type, Authorization',
      },
    );
  };
}
