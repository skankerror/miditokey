/* File: miditokey.c
 * Redirect f#2 g2 et g#2 in 'p' ' ' et 'n'
 * pour commander vlc avec un clavier midi.
 *
 * Compile with:
 * gcc -lxdo -lasound miditokey.c -o miditokey
 *
 * Requires libxdo (from xdotool project) 
 * and libasound from alsa project.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <alsa/asoundlib.h>
#include <xdo.h>

snd_seq_t *open_seq();
void midi_action(snd_seq_t *seq_handle);

// open a sequencer
snd_seq_t *open_seq() {

  snd_seq_t *seq_handle;
  int portid;

  if (snd_seq_open(&seq_handle, "default", SND_SEQ_OPEN_INPUT, 0) < 0) {
    fprintf(stderr, "Error opening ALSA sequencer.\n");
    exit(1);
  }
  snd_seq_set_client_name(seq_handle, "macbeth");
  if ((portid = snd_seq_create_simple_port(seq_handle, "macbeth",
            SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_WRITE,
            SND_SEQ_PORT_TYPE_APPLICATION)) < 0) {
    fprintf(stderr, "Error creating sequencer port.\n");
    exit(1);
  }
  return(seq_handle);
}

// listen midi events
void midi_action(snd_seq_t *seq_handle) {

  snd_seq_event_t *ev;

  do {
    snd_seq_event_input(seq_handle, &ev);
    if (ev->type==SND_SEQ_EVENT_NOTEON) {
        if (ev->data.note.note==9) {
			xdo_t *xdo = xdo_new(NULL);
			xdo_send_keysequence_window(xdo, CURRENTWINDOW,
                            "p", 12000);
					}
		if  (ev->data.note.note==65) {
			xdo_t *xdo = xdo_new(NULL);
			xdo_send_keysequence_window(xdo, CURRENTWINDOW,
                            "space", 12000);
		}
			
		if  (ev->data.note.note==1) {
				xdo_t *xdo = xdo_new(NULL);
				xdo_send_keysequence_window(xdo, CURRENTWINDOW,
                            "n", 12000);
		}           
    }
    snd_seq_free_event(ev);
  } while (snd_seq_event_input_pending(seq_handle, 0) > 0);
}

int main(int argc, char *argv[]) {

  snd_seq_t *seq_handle;
  int npfd;
  struct pollfd *pfd;
    
  seq_handle = open_seq();
  npfd = snd_seq_poll_descriptors_count(seq_handle, POLLIN);
  pfd = (struct pollfd *)alloca(npfd * sizeof(struct pollfd));
  snd_seq_poll_descriptors(seq_handle, pfd, npfd, POLLIN);
  while (1) {
    if (poll(pfd, npfd, 100000) > 0) {
      midi_action(seq_handle);
    }  
  }
}
