(function ($) {

Drupal.behaviors.views_mosaic = {
  attach: function (context, settings) {
    if(settings.mosaic) {
      //setting width/height of mosaic block as same backdrop image        
      $('.mosaic-block').each(function(){
          $(this).addClass(settings.mosaic.theme);
          $(this).width($(this).find('.mosaic-backdrop img').width());
          $(this).height($(this).find('.mosaic-backdrop img').height());
      });
      
      //custom doesn't apply only styles choosen by user
      if(settings.mosaic.theme == 'custom') {
          $('.mosaic-block').mosaic({
            animation : settings.mosaic.animation,
            speed: settings.mosaic.speed,
            opacity: settings.mosaic.opacity,
            preload : settings.mosaic.preload,
            anchor_x : settings.mosaic.anchor_x,
            anchor_y : settings.mosaic.anchor_y,
            hover_x : settings.mosaic.hover_x,
            hover_y : settings.mosaic.hover_y,
          });
      }
      else {        
        $('.mosaic-block.circle').mosaic({
          opacity: settings.mosaic.opacity,
          speed: settings.mosaic.speed,
        }); 

        $('.mosaic-block.fade').mosaic();

        $('.mosaic-block.bar').mosaic({
            animation : 'slide'   //fade or slide
        });

        $('.mosaic-block.bar2').mosaic({
          animation : 'slide'   //fade or slide
        });

        $('.mosaic-block.bar3').mosaic({
          animation : 'slide',  //fade or slide
          anchor_y  : 'top'   //Vertical anchor position
        });
        
        $('.mosaic-block.cover').mosaic({
          animation : 'slide',  //fade or slide
          hover_x   : '100%'   //Horizontal position on hover
        });
        
        $('.mosaic-block.cover2').mosaic({
          animation : 'slide',  //fade or slide
          anchor_y  : 'top',    //Vertical anchor position
          hover_y   : '80px'    //Vertical position on hover
        });
        
        $('.mosaic-block.cover3').mosaic({
          animation : 'slide',  //fade or slide
          hover_x   : '100%',  //Horizontal position on hover
          hover_y   : '120%'   //Vertical position on hover
        });
      }      
    }
  }
};

}(jQuery));
