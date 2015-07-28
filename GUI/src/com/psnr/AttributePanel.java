package com.psnr;

import java.awt.Color;
import java.awt.FlowLayout;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.UIManager.LookAndFeelInfo;

public class AttributePanel extends JFrame implements WindowListener {

	private static final long serialVersionUID = 7906101725848302884L;
	
	private AttributePanel attr_panel;
	private JPanel bottom_panel;
	
	public AttributePanel() {
		super("PSNR Calculator");
		
		setLayout(new FlowLayout());
		
		attr_panel = new AttributePanel();
		add(attr_panel);
		
		bottom_panel = new JPanel();
		add(bottom_panel);
		
		addWindowListener(this);
	}
	
	public void setLookAndFeel() {
		getContentPane().setBackground(new Color(193, 203, 247));

		try {
			for (LookAndFeelInfo info : UIManager.getInstalledLookAndFeels()) {
				if ("Nimbus".equals(info.getName())) {
					UIManager.setLookAndFeel(info.getClassName());
					break;
				}
			}
		} catch (UnsupportedLookAndFeelException e) {
			JOptionPane.showMessageDialog(null, "UnsupportedLookAndFeelException", "Nimbus", 0);
		} catch (ClassNotFoundException e) {
			JOptionPane.showMessageDialog(null, "ClassNotFoundException", "Nimbus", 0);
		} catch (InstantiationException e) {
			JOptionPane.showMessageDialog(null, "InstantiationException", "Nimbus", 0);
		} catch (IllegalAccessException e) {
			JOptionPane.showMessageDialog(null, "IllegalAccessException", "Nimbus", 0);
		}
	}


	@Override	public void windowActivated(WindowEvent arg0) { }
	@Override	public void windowClosed(WindowEvent arg0) { }
	@Override	public void windowClosing(WindowEvent arg0) { }
	@Override	public void windowDeactivated(WindowEvent arg0) { }
	@Override	public void windowDeiconified(WindowEvent arg0) { }
	@Override	public void windowIconified(WindowEvent arg0) { }
	@Override	public void windowOpened(WindowEvent arg0) { }
	
}
