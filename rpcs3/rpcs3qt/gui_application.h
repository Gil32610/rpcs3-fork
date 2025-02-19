#pragma once

#include "util/types.hpp"
#include "util/atomic.hpp"

#include <QApplication>
#include <QElapsedTimer>
#include <QTimer>
#include <QTranslator>

#include "main_application.h"

#include "Emu/System.h"

#include <memory>
#include <functional>

class gs_frame;
class main_window;
class gui_settings;
class emu_settings;
class persistent_settings;

/** RPCS3 GUI Application Class
 * The main point of this class is to do application initialization, to hold the main and game windows and to initialize callbacks.
 */
class gui_application : public QApplication, public main_application
{
	Q_OBJECT
public:
	gui_application(int& argc, char** argv);
	~gui_application();

	void SetShowGui(bool show_gui = true)
	{
		m_show_gui = show_gui;
	}

	void SetUseCliStyle(bool use_cli_style = false)
	{
		m_use_cli_style = use_cli_style;
	}

	void SetWithCliBoot(bool with_cli_boot = false)
	{
		m_with_cli_boot = with_cli_boot;
	}

	void SetStartGamesFullscreen(bool start_games_fullscreen = false)
	{
		m_start_games_fullscreen = start_games_fullscreen;
	}

	void SetGameScreenIndex(int screen_index = -1)
	{
		m_game_screen_index = screen_index;
	}

	/** Call this method before calling app.exec */
	bool Init() override;

	std::unique_ptr<gs_frame> get_gs_frame();

	main_window* m_main_window = nullptr;

private:
	QThread* get_thread() override
	{
		return thread();
	}

	void SwitchTranslator(QTranslator& translator, const QString& filename, const QString& language_code);
	void LoadLanguage(const QString& language_code);
	static QStringList GetAvailableLanguageCodes();

	void InitializeCallbacks();
	void InitializeConnects();

	void StartPlaytime(bool start_playtime);
	void UpdatePlaytime();
	void StopPlaytime();

	QTranslator m_translator;
	QTranslator m_translator_qt;
	QString m_language_code;

	QTimer m_timer;
	QElapsedTimer m_timer_playtime;

	std::shared_ptr<emu_settings> m_emu_settings;
	std::shared_ptr<gui_settings> m_gui_settings;
	std::shared_ptr<persistent_settings> m_persistent_settings;

	bool m_show_gui = true;
	bool m_use_cli_style = false;
	bool m_with_cli_boot = false;
	bool m_start_games_fullscreen = false;
	int m_game_screen_index = -1;

	u64 m_pause_amend_time_on_focus_loss = umax;
	u64 m_pause_delayed_tag = 0;
	typename Emulator::stop_counter_t m_emu_focus_out_emulation_id{};
	bool m_is_pause_on_focus_loss_active = false;

private Q_SLOTS:
	void OnChangeStyleSheetRequest();
	void OnAppStateChanged(Qt::ApplicationState state);

Q_SIGNALS:
	void OnEmulatorRun(bool start_playtime);
	void OnEmulatorPause();
	void OnEmulatorResume(bool start_playtime);
	void OnEmulatorStop();
	void OnEmulatorReady();
	void OnEnableDiscEject(bool enabled);
	void OnEnableDiscInsert(bool enabled);

	void RequestCallFromMainThread(std::function<void()> func, atomic_t<bool>* wake_up);

private Q_SLOTS:
	static void CallFromMainThread(const std::function<void()>& func, atomic_t<bool>* wake_up);
};
