// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "CoreApplicationPlugin.hpp"
#include "AboutDialog.hpp"

#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <core/view/QRecentFilesMenu.h>
#include <core/presenter/CoreActions.hpp>
#include <core/settings/Settings.hpp>
#include <core/settings/SettingsView.hpp>
#include <core/view/Window.hpp>
#include <score/actions/Menu.hpp>
#include <score/plugins/documentdelegate/DocumentDelegateFactory.hpp>
#include <score_git_info.hpp>

SCORE_DECLARE_ACTION(Documentation, "&Documentation", Common, QKeySequence::UnknownKey)
SCORE_DECLARE_ACTION(Issues, "&Report Issues", Common, QKeySequence::UnknownKey)
SCORE_DECLARE_ACTION(Forum, "&Forum", Common, QKeySequence::UnknownKey)

namespace score
{

CoreApplicationPlugin::CoreApplicationPlugin(
    const GUIApplicationContext& app, Presenter& pres)
    : GUIApplicationPlugin{app}, m_presenter{pres}
{
}

void CoreApplicationPlugin::newDocument()
{
  m_presenter.m_docManager.newDocument(
      context, getStrongId(m_presenter.m_docManager.documents()),
      *context.interfaces<score::DocumentDelegateList>().begin());
}

void CoreApplicationPlugin::load()
{
  m_presenter.m_docManager.loadFile(context);
}

void CoreApplicationPlugin::save()
{
  m_presenter.m_docManager.saveDocument(
      *m_presenter.m_docManager.currentDocument());
}

void CoreApplicationPlugin::saveAs()
{
  m_presenter.m_docManager.saveDocumentAs(
      *m_presenter.m_docManager.currentDocument());
}

void CoreApplicationPlugin::close()
{
  if (auto doc = m_presenter.m_docManager.currentDocument())
    m_presenter.m_docManager.closeDocument(context, *doc);
}

void CoreApplicationPlugin::quit()
{
  if (m_presenter.m_view)
    m_presenter.m_view->close();
}

void CoreApplicationPlugin::restoreLayout()
{
  if (m_presenter.m_view)
    m_presenter.m_view->restoreLayout();
}

void CoreApplicationPlugin::openSettings()
{
  m_presenter.m_settings.view().exec();
}
void CoreApplicationPlugin::openProjectSettings()
{
  auto doc = m_presenter.documentManager().currentDocument();
  if (doc)
  {
    m_presenter.m_projectSettings.setup(doc->context());
    m_presenter.m_projectSettings.view().exec();
  }
}

void CoreApplicationPlugin::help()
{
   QDesktopServices::openUrl(QUrl("https://ossia.github.io/score"));
}

void CoreApplicationPlugin::about()
{
  AboutDialog aboutDialog;
  aboutDialog.exec();
  aboutDialog.deleteLater();
}

void CoreApplicationPlugin::loadStack()
{
  m_presenter.m_docManager.loadStack(context);
}

void CoreApplicationPlugin::saveStack()
{
  m_presenter.m_docManager.saveStack();
}

GUIElements CoreApplicationPlugin::makeGUIElements()
{
  GUIElements e;
  auto& menus = e.menus;
  auto& actions = e.actions.container;
  (void)actions;

  menus.reserve(10);
  auto file = new QMenu{tr("&Fichier")};
  auto edit = new QMenu{tr("&Édition")};
  auto view = new QMenu{tr("&Vue")};
  auto settings = new QMenu{tr("&Paramètres")};
  auto about = new QMenu{tr("&Aide")};
  menus.emplace_back(file, Menus::File(), Menu::is_toplevel{}, 0);
  menus.emplace_back(edit, Menus::Edit(), Menu::is_toplevel{}, 1);
  menus.emplace_back(view, Menus::View(), Menu::is_toplevel{}, 5);
  menus.emplace_back(settings, Menus::Settings(), Menu::is_toplevel{}, 6);

  // Menus are by default at int_max - 1 so that they will be sorted before
  menus.emplace_back(
      about, Menus::About(), Menu::is_toplevel{},
      std::numeric_limits<int>::max());

  auto export_menu = new QMenu{tr("&Export")};
  menus.emplace_back(export_menu, Menus::Export());

  auto windows_menu = new QMenu{tr("&Fenêtres")};
  menus.emplace_back(windows_menu, Menus::Windows());

  ////// File //////
  // New
  // ---
  // Load
  // Recent
  // Save
  // Save As
  // ---
  // Export
  // ---
  // Close
  // Quit

  if (m_presenter.view())
  {

    {
      auto new_doc = new QAction(m_presenter.view());
      connect(
          new_doc, &QAction::triggered, this,
          &CoreApplicationPlugin::newDocument);
      file->addAction(new_doc);
      e.actions.add<Actions::New>(new_doc);
    }

    file->addSeparator();

    {
      auto load_doc = new QAction(m_presenter.view());
      connect(
          load_doc, &QAction::triggered, this, &CoreApplicationPlugin::load);
      e.actions.add<Actions::Load>(load_doc);
      file->addAction(load_doc);
    }

    file->addMenu(m_presenter.m_docManager.recentFiles());

    auto& cond = context.actions.condition<EnableActionIfDocument>();
    {
      auto save_doc = new QAction(m_presenter.view());
      connect(
          save_doc, &QAction::triggered, this, &CoreApplicationPlugin::save);
      e.actions.add<Actions::Save>(save_doc);
      cond.add<Actions::Save>();
      file->addAction(save_doc);
    }

    {
      auto saveas_doc = new QAction(m_presenter.view());
      connect(
          saveas_doc, &QAction::triggered, this,
          &CoreApplicationPlugin::saveAs);
      e.actions.add<Actions::SaveAs>(saveas_doc);
      cond.add<Actions::SaveAs>();
      file->addAction(saveas_doc);
    }

    file->addSeparator();

    file->addMenu(export_menu);
#ifdef SCORE_DEBUG
    // Add command stack import / export
    {
      auto loadStack_act = new QAction(m_presenter.view());
      connect(
          loadStack_act, &QAction::triggered, this,
          &CoreApplicationPlugin::loadStack);
      actions.emplace_back(
          loadStack_act, tr("&Load a stack"), "LoadStack", "Common",
          QKeySequence::UnknownKey);
      export_menu->addAction(loadStack_act);
    }

    {
      auto saveStack_act = new QAction(m_presenter.view());
      connect(
          saveStack_act, &QAction::triggered, this,
          &CoreApplicationPlugin::saveStack);
      actions.emplace_back(
          saveStack_act, tr("&Save a stack"), "SaveStack", "Common",
          QKeySequence::UnknownKey);
      export_menu->addAction(saveStack_act);
    }
#endif

    file->addSeparator();

    {
      auto close_act = new QAction(m_presenter.view());
      connect(
          close_act, &QAction::triggered, this, &CoreApplicationPlugin::close);
      e.actions.add<Actions::Close>(close_act);
      file->addAction(close_act);
    }

    {
      auto quit_act = new QAction(m_presenter.view());
      connect(
          quit_act, &QAction::triggered, this, &CoreApplicationPlugin::quit);
      e.actions.add<Actions::Quit>(quit_act);
      file->addAction(quit_act);
    }

    ////// View //////
    view->addMenu(windows_menu);
    {
      auto act = new QAction(m_presenter.view());
      connect(
          act, &QAction::triggered, this,
          &CoreApplicationPlugin::restoreLayout);
      e.actions.add<Actions::RestoreLayout>(act);
      windows_menu->addAction(act);
    }

    ////// Settings //////
    {
      auto settings_act = new QAction(m_presenter.view());
      connect(
          settings_act, &QAction::triggered, this,
          &CoreApplicationPlugin::openSettings);
      e.actions.add<Actions::OpenSettings>(settings_act);
      settings->addAction(settings_act);
    }

    ////// About /////
    {
      auto help_act = new QAction(m_presenter.view());
      connect(
          help_act, &QAction::triggered, this, &CoreApplicationPlugin::help);
      e.actions.add<Actions::Help>(help_act);
      about->addAction(help_act);
    }
  }

  return e;
}
}
