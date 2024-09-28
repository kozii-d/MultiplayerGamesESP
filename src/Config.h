#ifndef CONFIG_H
#define CONFIG_H

// Настройки EncButton
#define EB_NO_FOR      // отключить поддержку pressFor/holdFor/stepFor и счётчик степов (экономит 2 байта оперативки)
#define EB_NO_CALLBACK // отключить обработчик событий attach (экономит 2 байта оперативки)
#define EB_NO_COUNTER  // отключить счётчик энкодера (экономит 4 байта оперативки)
#define EB_NO_BUFFER   // отключить буферизацию энкодера (экономит 1 байт оперативки)

// #define EB_DEB_TIME 50      // таймаут гашения дребезга кнопки (кнопка)
// #define EB_CLICK_TIME 500   // таймаут ожидания кликов (кнопка)
// #define EB_HOLD_TIME 600    // таймаут удержания (кнопка)
// #define EB_STEP_TIME 25    // таймаут импульсного удержания (кнопка)

#endif