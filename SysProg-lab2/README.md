# SysProg_Lab2
Добавить в приложения из первой лабораторной работы передачу информации с помощью файла, отображаемого в память (memory mapped file). Реализовать транспортные функции в виде динамически подключаемой библиотеки (C++) с неявной загрузкой в консольное приложение и подключением в диалог с помощью директивы DllImport. Потоковые функции остаются в исполняемом файле.

При нажатии кнопки "Send" диалоговое приложение пересылает консоли текст произвольной длины, введенный в поле ввода. Список/выпадающий список задает поток-адресат. При выборе строки "Все потоки" сообщение отправляется всем потокам, каждый поток считывает информацию независимо от других потоков.

Рабочие потоки создаются с помощью функции AfxBeginThread. 

Главный поток выводит полученный текст на стандартный вывод (stdout), рабочие потоки создают текстовые файлы с именами <свой номер>.txt и дописывают в них полученный текст. Файл, отображаемый в память, защищается от совместного использования с помощью объекта синхронизации mutex.

Все действия подтверждаются сообщениями от консоли.

Обратите внимание: условия работы отличаются от заданий предыдущих лет. Отправка прошлогоднего кода гарантирует -20% с уходом на переделывание.

И обратите еще большее внимание: задание сформулировано как "Добавить в приложения из первой лабораторной работы...", а не "Взять первую попавшуюся чужую лабораторную работу...". Т.е. вторая работа должна быть развитием первой, иначе - см. предыдущий абзац.