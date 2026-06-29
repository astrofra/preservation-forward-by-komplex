import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.Font;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;
import javax.swing.BorderFactory;
import javax.swing.ButtonGroup;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JComboBox;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JRadioButton;
import javax.swing.SwingUtilities;
import javax.swing.UIManager;

final class ForwardStartupDialog {
    private ForwardStartupDialog() {
    }

    static String[] maybeShow(String[] stringArray) {
        if (!ForwardStartupDialog.shouldShow(stringArray)) {
            return stringArray;
        }
        ForwardStartupSelection forwardStartupSelection = ForwardStartupDialog.showDialog(stringArray);
        if (forwardStartupSelection == null) {
            System.exit(0);
        }
        return ForwardStartupDialog.appendSelection(stringArray, forwardStartupSelection);
    }

    private static boolean shouldShow(String[] stringArray) {
        if (stringArray == null || stringArray.length == 0) {
            return true;
        }
        int n = 0;
        while (n < stringArray.length - 1) {
            String string = stringArray[n];
            String string2 = stringArray[n + 1];
            if (string == null) {
                n += 2;
                continue;
            }
            String string3 = string.trim().toLowerCase(Locale.ROOT);
            if ("capture".equals(string3) || "captureexit".equals(string3) || "captureevery".equals(string3) || "captureintervalms".equals(string3) || "capturestartms".equals(string3) || "capturestopms".equals(string3) || "capturelimit".equals(string3)) {
                return false;
            }
            if (ForwardLaunchConfiguration.PARAM_DISPLAY_MODE.equals(string3) || ForwardLaunchConfiguration.PARAM_DISPLAY_SCALE.equals(string3) || ForwardLaunchConfiguration.PARAM_DISPLAY_WIDTH.equals(string3) || ForwardLaunchConfiguration.PARAM_DISPLAY_HEIGHT.equals(string3) || ForwardLaunchConfiguration.PARAM_FULLSCREEN.equals(string3)) {
                return false;
            }
            if (ForwardLaunchConfiguration.PARAM_LAUNCHER.equals(string3) && !ForwardStartupDialog.isTruthy(string2)) {
                return false;
            }
            n += 2;
        }
        return true;
    }

    private static ForwardStartupSelection showDialog(String[] stringArray) {
        try {
            UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
        }
        catch (Exception exception) {
        }
        final ForwardStartupSelection[] forwardStartupSelectionArray = new ForwardStartupSelection[1];
        final boolean[] blArray = new boolean[1];
        Runnable runnable = new Runnable(){

            public void run() {
                forwardStartupSelectionArray[0] = ForwardStartupDialog.createAndShowDialog(stringArray, blArray);
            }
        };
        try {
            if (SwingUtilities.isEventDispatchThread()) {
                runnable.run();
            } else {
                SwingUtilities.invokeAndWait(runnable);
            }
        }
        catch (Exception exception) {
            exception.printStackTrace();
            return null;
        }
        if (blArray[0]) {
            return forwardStartupSelectionArray[0];
        }
        return null;
    }

    private static ForwardStartupSelection createAndShowDialog(String[] stringArray, boolean[] blArray) {
        final JDialog jDialog = new JDialog();
        jDialog.setTitle(ForwardLaunchConfiguration.APPLICATION_TITLE);
        jDialog.setModal(true);
        jDialog.setDefaultCloseOperation(2);
        JPanel jPanel = new JPanel(new BorderLayout(0, 14));
        jPanel.setBorder(BorderFactory.createEmptyBorder(14, 16, 14, 16));
        JLabel jLabel = new JLabel("Display options:");
        jLabel.setFont(jLabel.getFont().deriveFont(Font.BOLD, jLabel.getFont().getSize2D() + 1.0f));
        jPanel.add((Component)jLabel, "North");
        JPanel jPanel2 = new JPanel(new GridBagLayout());
        GridBagConstraints gridBagConstraints = new GridBagConstraints();
        gridBagConstraints.insets = new Insets(4, 0, 4, 0);
        gridBagConstraints.anchor = 17;
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.fill = 2;
        JRadioButton jRadioButton = new JRadioButton("Windowed", true);
        JRadioButton jRadioButton2 = new JRadioButton("Fullscreen");
        ButtonGroup buttonGroup = new ButtonGroup();
        buttonGroup.add(jRadioButton);
        buttonGroup.add(jRadioButton2);
        jPanel2.add((Component)jRadioButton, gridBagConstraints);
        gridBagConstraints.gridy = 1;
        jPanel2.add((Component)jRadioButton2, gridBagConstraints);
        gridBagConstraints.gridy = 2;
        gridBagConstraints.insets = new Insets(12, 0, 4, 8);
        gridBagConstraints.fill = 0;
        gridBagConstraints.weightx = 0.0;
        jPanel2.add((Component)new JLabel("Display size:"), gridBagConstraints);
        gridBagConstraints.gridx = 1;
        gridBagConstraints.insets = new Insets(12, 0, 4, 0);
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.fill = 2;
        final List<ForwardDisplayOptions.ResolutionOption> list = ForwardDisplayOptions.loadResolutions();
        final JComboBox<ForwardDisplayOptions.ResolutionOption> jComboBox = new JComboBox<ForwardDisplayOptions.ResolutionOption>(list.toArray(new ForwardDisplayOptions.ResolutionOption[0]));
        jPanel2.add((Component)jComboBox, gridBagConstraints);
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 3;
        gridBagConstraints.gridwidth = 2;
        gridBagConstraints.insets = new Insets(12, 0, 0, 0);
        final JCheckBox jCheckBox = new JCheckBox("Use original 1x1 pixel mode");
        jCheckBox.setSelected(ForwardStartupDialog.has1x1Flag(stringArray));
        jPanel2.add((Component)jCheckBox, gridBagConstraints);
        jPanel.add((Component)jPanel2, "Center");
        JPanel jPanel3 = new JPanel(new FlowLayout(2, 8, 0));
        JButton jButton = new JButton("Quit :(");
        JButton jButton2 = new JButton("Start Demo :)");
        jPanel3.add(jButton);
        jPanel3.add(jButton2);
        jPanel.add((Component)jPanel3, "South");
        jDialog.setContentPane(jPanel);
        jDialog.getRootPane().setDefaultButton(jButton2);
        final ForwardStartupSelection[] forwardStartupSelectionArray = new ForwardStartupSelection[1];
        jButton2.addActionListener(actionEvent -> {
            ForwardDisplayOptions.ResolutionOption resolutionOption = (ForwardDisplayOptions.ResolutionOption)jComboBox.getSelectedItem();
            if (resolutionOption == null) {
                resolutionOption = ForwardDisplayOptions.defaultResolutions().get(0);
            }
            forwardStartupSelectionArray[0] = new ForwardStartupSelection(jRadioButton2.isSelected(), resolutionOption.width, resolutionOption.height, jCheckBox.isSelected());
            blArray[0] = true;
            jDialog.dispose();
        });
        jButton.addActionListener(actionEvent -> {
            blArray[0] = false;
            jDialog.dispose();
        });
        jDialog.addWindowListener(new WindowAdapter(){

            public void windowClosing(WindowEvent windowEvent) {
                blArray[0] = false;
            }
        });
        jDialog.pack();
        jDialog.setMinimumSize(new Dimension(380, jDialog.getHeight()));
        jDialog.setLocationRelativeTo(null);
        jDialog.setVisible(true);
        return forwardStartupSelectionArray[0];
    }

    private static String[] appendSelection(String[] stringArray, ForwardStartupSelection forwardStartupSelection) {
        List<String> list = new ArrayList<String>();
        if (stringArray != null) {
            int n = 0;
            while (n < stringArray.length) {
                if (n < stringArray.length - 1 && ForwardStartupDialog.isLauncherControlParameter(stringArray[n])) {
                    n += 2;
                    continue;
                }
                list.add(stringArray[n]);
                ++n;
            }
        }
        list.add(ForwardLaunchConfiguration.PARAM_DISPLAY_MODE);
        list.add(forwardStartupSelection.fullscreen ? ForwardLaunchConfiguration.MODE_FULLSCREEN : ForwardLaunchConfiguration.MODE_WINDOWED);
        list.add(ForwardLaunchConfiguration.PARAM_DISPLAY_WIDTH);
        list.add(String.valueOf(forwardStartupSelection.displayWidth));
        list.add(ForwardLaunchConfiguration.PARAM_DISPLAY_HEIGHT);
        list.add(String.valueOf(forwardStartupSelection.displayHeight));
        list.add("1x1");
        list.add(forwardStartupSelection.pixelMode1x1 ? "1" : "0");
        list.add(ForwardLaunchConfiguration.PARAM_LAUNCHER);
        list.add("0");
        return list.toArray(new String[0]);
    }

    private static boolean has1x1Flag(String[] stringArray) {
        if (stringArray == null) {
            return true;
        }
        int n = 0;
        while (n < stringArray.length - 1) {
            if ("1x1".equalsIgnoreCase(stringArray[n])) {
                return !ForwardLaunchConfiguration.isFalsey(stringArray[n + 1]);
            }
            n += 2;
        }
        return true;
    }

    private static boolean isTruthy(String string) {
        if (string == null) {
            return false;
        }
        String string2 = string.trim().toLowerCase(Locale.ROOT);
        return string2.equals("1") || string2.equals("true") || string2.equals("yes") || string2.equals("on");
    }

    private static boolean isLauncherControlParameter(String string) {
        if (string == null) {
            return false;
        }
        String string2 = string.trim().toLowerCase(Locale.ROOT);
        return "1x1".equals(string2) || ForwardLaunchConfiguration.PARAM_DISPLAY_MODE.equals(string2) || ForwardLaunchConfiguration.PARAM_DISPLAY_SCALE.equals(string2) || ForwardLaunchConfiguration.PARAM_DISPLAY_WIDTH.equals(string2) || ForwardLaunchConfiguration.PARAM_DISPLAY_HEIGHT.equals(string2) || ForwardLaunchConfiguration.PARAM_FULLSCREEN.equals(string2) || ForwardLaunchConfiguration.PARAM_LAUNCHER.equals(string2);
    }

    private static final class ForwardStartupSelection {
        final boolean fullscreen;
        final int displayWidth;
        final int displayHeight;
        final boolean pixelMode1x1;

        ForwardStartupSelection(boolean bl, int n, int n2, boolean bl2) {
            this.fullscreen = bl;
            this.displayWidth = n;
            this.displayHeight = n2;
            this.pixelMode1x1 = bl2;
        }
    }
}
